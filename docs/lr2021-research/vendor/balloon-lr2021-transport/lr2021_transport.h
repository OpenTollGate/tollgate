/**
 * @file lr2021_transport.h
 * @brief LR2021 SPI transport adapter — combines SPI + framing for stream I/O.
 *
 * Ported from Rust microfips-esp-transport: lr2021_transport.rs
 *
 * Implements the Transport stream interface (send/recv byte slices) using the
 * LR2021 (Semtech dual-band LoRa/FLRC radio) over SPI.
 *
 * ## Architecture
 *
 *  ┌──────────────────┐     send()/recv()
 *  │  FIPS Protocol   │ ◄──────────────────►  Lr2021Transport
 *  │  (FrameWriter)   │                        │
 *  └──────────────────┘                        │
 *                               ┌──────────────┴──────────────┐
 *                               │  TxFramer / RxFramer         │
 *                               │  (stream ↔ packet adapter)   │
 *                               └──────────────┬──────────────┘
 *                                              │
 *                               ┌──────────────┴──────────────┐
 *                               │  LR2021 SPI Driver           │
 *                               │  (register config + TX/RX)   │
 *                               └──────────────┬──────────────┘
 *                                              │ SPI bus
 *                               ┌──────────────┴──────────────┐
 *                               │  ESP-IDF SPI + GPIO          │
 *                               │  + GPIO (CS, BUSY, DIO, RST) │
 *                               └──────────────────────────────┘
 *
 * ## FLRC Configuration (proven baseline)
 * - Frequency: 2440 MHz (2.4 GHz ISM)
 * - Bitrate: 2600 kbps (FLRC)
 * - Payload: up to 255 bytes per packet
 * - TX Power: +12 dBm
 * - 0% packet loss at bench distance
 *
 * Ported per ADR-024 extract operation from microfips reference repo.
 */

#ifndef LR2021_TRANSPORT_H
#define LR2021_TRANSPORT_H

#include <stdint.h>
#include <stddef.h>
#include <vector>

#include "lr2021_spi.h"
#include "lr2021_framing.h"

#ifdef __cplusplus

/// Default timeout for radio operations (handshake, packet TX/RX) in milliseconds
static constexpr uint32_t RADIO_TIMEOUT_MS = 5000;

/// Poll interval when waiting for IRQ in milliseconds
static constexpr uint32_t IRQ_POLL_MS = 1;

/**
 * Error type for the transport layer.
 * Ported from Rust TransportError enum.
 */
enum class TransportError {
    Ok,              ///< Success
    Radio,           ///< Radio hardware error (SPI failure, timeout, etc.)
    NotInitialized,  ///< Radio not initialized — call init() first
    Timeout,         ///< Operation timed out (no packet received, TX never completed)
    CrcError,        ///< Received packet was corrupt (CRC mismatch)
};

/// Convenience: check if a TransportError represents success
inline bool transport_ok(TransportError e) { return e == TransportError::Ok; }

/**
 * LR2021 Transport — stream interface (send/recv) over the packet-based FLRC link.
 *
 * Wraps the LR2021 radio with the framing layer. Provides a blocking
 * send/recv byte-slice interface suitable for FIPS protocol FrameWriter/FrameReader.
 *
 * Usage:
 * @code
 *   auto radio = std::make_unique<MockLr2021Radio>();
 *   Lr2021Transport transport(radio.get());
 *   transport.init(Lr2021Config{});
 *   transport.send(data, len);
 *   transport.flush_tx();
 *   transport.recv(buf, buf_len, &n);
 * @endcode
 */
class Lr2021Transport {
public:
    /**
     * Create a new LR2021 transport wrapper.
     *
     * @param radio Pointer to a Lr2021Radio implementation (Mock or EspIdf).
     *              Caller retains ownership; transport does not free it.
     */
    explicit Lr2021Transport(Lr2021Radio* radio)
        : radio_(radio)
        , initialized_(false)
    {}

    /**
     * Initialize the radio with FLRC configuration.
     * Must be called before any send/recv operations.
     * Configures: frequency, bitrate, TX power, sync word, packet mode.
     */
    TransportError init(const Lr2021Config& config) {
        if (radio_->init(config) != Lr2021Error::Ok)
            return TransportError::Radio;
        if (radio_->start_rx() != Lr2021Error::Ok)
            return TransportError::Radio;
        initialized_ = true;
        return TransportError::Ok;
    }

    /**
     * Handle an IRQ from the radio's DIO pin.
     * This should be called from the GPIO interrupt handler (or polled).
     * Reads IRQ status, clears flags, buffers received packets.
     */
    TransportError handle_irq();

    /**
     * Send data over the transport (stream interface).
     * Data is pushed into the TX framer. May produce multiple radio packets
     * if data exceeds MAX_PACKET. Does NOT automatically flush — call
     * flush_tx() to transmit remaining buffered bytes.
     *
     * @param data Bytes to send
     * @param len  Number of bytes
     * @return TransportError::Ok on success
     */
    TransportError send(const uint8_t* data, size_t len);

    /**
     * Receive data from the transport (stream interface).
     * Drains up to buf_len bytes from the RX framer. If no data is buffered,
     * waits for the next packet (polls IRQ up to timeout_ms).
     *
     * @param buf        Output buffer
     * @param buf_len    Size of output buffer
     * @param n_out      [out] Number of bytes received
     * @param timeout_ms Maximum time to wait for data (default: RADIO_TIMEOUT_MS)
     * @return TransportError::Ok on success, Timeout if no data received
     */
    TransportError recv(uint8_t* buf, size_t buf_len, size_t* n_out,
                        uint32_t timeout_ms = RADIO_TIMEOUT_MS);

    /**
     * Flush any pending TX data as a (possibly short) packet.
     * Called when the upper layer wants to ensure data is transmitted immediately.
     */
    TransportError flush_tx();

    /**
     * Poll the IRQ pin (alternative to interrupt-driven handle_irq).
     * Call this in a loop if DIO interrupts are not configured.
     */
    TransportError poll_irq();

    /// Check if radio is initialized and ready
    bool is_initialized() const { return initialized_; }

    /// Access the underlying radio (for test inspection)
    Lr2021Radio* radio() { return radio_; }

private:
    /**
     * Transmit a single FLRC packet and wait for TX_DONE.
     */
    TransportError transmit_packet(const uint8_t* data, size_t len);

    Lr2021Radio* radio_;
    TxFramer tx_framer_;
    RxFramer rx_framer_;
    bool initialized_;
};

#endif // __cplusplus
#endif // LR2021_TRANSPORT_H
