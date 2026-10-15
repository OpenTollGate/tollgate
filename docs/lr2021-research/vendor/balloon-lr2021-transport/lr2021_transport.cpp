/**
 * @file lr2021_transport.cpp
 * @brief LR2021 transport implementation — combines SPI + framing for stream I/O.
 *
 * Ported from Rust microfips-esp-transport: lr2021_transport.rs
 *
 * Wraps the LR2021 radio (Lr2021Radio interface) with the framing layer
 * (TxFramer / RxFramer), providing a blocking stream interface (send/recv)
 * over the packet-based FLRC link.
 *
 * Ported per ADR-024 extract operation from microfips reference repo.
 */

#include "lr2021_transport.h"

// ═══════════════════════════════════════════════════════════════════
// Lr2021Transport implementation
// ═══════════════════════════════════════════════════════════════════

TransportError Lr2021Transport::handle_irq() {
    uint32_t irq = 0;
    if (radio_->get_irq_status(irq) != Lr2021Error::Ok)
        return TransportError::Radio;

    if (IrqSource::contains(irq, IrqSource::RX_DONE)) {
        // Read the received packet into the RX framer
        uint8_t pkt_buf[LR2021_FRAMING_MAX_PACKET];
        PacketStatus status;
        Lr2021Error err = radio_->read_packet(pkt_buf, sizeof(pkt_buf), status);
        if (err == Lr2021Error::Ok) {
            rx_framer_.push_packet(pkt_buf, status.length);
        } else {
            // Packet read failed — clear RX and restart
            if (radio_->start_rx() != Lr2021Error::Ok)
                return TransportError::Radio;
        }
    }

    if (IrqSource::contains(irq, IrqSource::TX_DONE)) {
        // Return to RX mode after TX
        if (radio_->start_rx() != Lr2021Error::Ok)
            return TransportError::Radio;
    }

    if (radio_->clear_irq() != Lr2021Error::Ok)
        return TransportError::Radio;

    return TransportError::Ok;
}

TransportError Lr2021Transport::send(const uint8_t* data, size_t len) {
    if (!initialized_)
        return TransportError::NotInitialized;

    // Push data into TX framer — may need multiple radio packets
    size_t offset = 0;
    while (offset < len) {
        size_t consumed = tx_framer_.push(data + offset, len - offset);

        // If buffer is full, flush a packet to the radio
        if (tx_framer_.is_full()) {
            uint8_t pkt[LR2021_FRAMING_MAX_PACKET];
            size_t n = tx_framer_.take_packet(pkt, sizeof(pkt));
            TransportError terr = transmit_packet(pkt, n);
            if (terr != TransportError::Ok)
                return terr;
        }

        offset += consumed;
    }

    return TransportError::Ok;
}

TransportError Lr2021Transport::recv(uint8_t* buf, size_t buf_len, size_t* n_out,
                                     uint32_t timeout_ms) {
    if (!initialized_)
        return TransportError::NotInitialized;

    // Try to drain from existing RX buffer first
    size_t n = rx_framer_.drain(buf, buf_len);
    if (n > 0) {
        *n_out = n;
        return TransportError::Ok;
    }

    // No data buffered — poll IRQ up to timeout_ms
    uint32_t elapsed = 0;
    while (elapsed < timeout_ms) {
        // Poll IRQ pin
        bool asserted = false;
        if (radio_->check_irq(asserted) != Lr2021Error::Ok)
            return TransportError::Radio;

        if (asserted) {
            TransportError terr = handle_irq();
            if (terr != TransportError::Ok)
                return terr;

            // Try draining again after handling IRQ
            n = rx_framer_.drain(buf, buf_len);
            if (n > 0) {
                *n_out = n;
                return TransportError::Ok;
            }
        }

        // Small delay between polls
        // On host: no delay (spin). On ESP-IDF: vTaskDelay.
#if defined(__has_include)
#if __has_include(<freertos/FreeRTOS.h>)
        vTaskDelay(pdMS_TO_TICKS(IRQ_POLL_MS));
#else
        // Host mode: busy-wait (tests use mock, so timeout is instant)
#endif
#endif
        elapsed += IRQ_POLL_MS;
    }

    *n_out = 0;
    return TransportError::Timeout;
}

TransportError Lr2021Transport::flush_tx() {
    if (!initialized_)
        return TransportError::NotInitialized;

    if (tx_framer_.has_pending()) {
        uint8_t pkt[LR2021_FRAMING_MAX_PACKET];
        size_t n = tx_framer_.take_packet(pkt, sizeof(pkt));
        if (n > 0) {
            return transmit_packet(pkt, n);
        }
    }
    return TransportError::Ok;
}

TransportError Lr2021Transport::transmit_packet(const uint8_t* data, size_t len) {
    Lr2021Error err = radio_->send_packet(data, len);
    if (err != Lr2021Error::Ok)
        return TransportError::Radio;

    // Poll IRQ until TX_DONE (mock sets it immediately, real radio takes ~ms)
    for (uint32_t i = 0; i < LR2021_TX_DONE_ITER; i++) {
        uint32_t irq = 0;
        err = radio_->get_irq_status(irq);
        if (err != Lr2021Error::Ok)
            return TransportError::Radio;

        if (IrqSource::contains(irq, IrqSource::TX_DONE)) {
            if (radio_->clear_irq() != Lr2021Error::Ok)
                return TransportError::Radio;
            radio_->start_rx();
            return TransportError::Ok;
        }

        // Yield to prevent watchdog timeout (fix: 8f93593).
        // send_packet() already delays 5ms after SET_TX, so TX_DONE is
        // typically set on the first poll. This vTaskDelay is a safety net.
#if defined(__has_include)
#if __has_include(<freertos/FreeRTOS.h>)
        vTaskDelay(pdMS_TO_TICKS(IRQ_POLL_MS));
#else
        // Host mode: busy-wait (mock sets TX_DONE immediately)
#endif
#endif
    }

    return TransportError::Timeout;
}

TransportError Lr2021Transport::poll_irq() {
    if (!initialized_)
        return TransportError::NotInitialized;

    bool asserted = false;
    if (radio_->check_irq(asserted) != Lr2021Error::Ok)
        return TransportError::Radio;

    if (asserted) {
        return handle_irq();
    }
    return TransportError::Ok;
}
