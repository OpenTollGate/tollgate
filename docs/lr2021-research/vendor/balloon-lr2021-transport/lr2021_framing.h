/**
 * @file lr2021_framing.h
 * @brief LR2021 framing layer — fragments byte streams into FLRC-sized packets
 *        and reassembles them back into a byte stream.
 *
 * Ported from Rust microfips-esp-transport: lr2021_framing.rs
 *
 * The Transport layer is stream-oriented (send/recv byte slices), but the
 * LR2021 radio transmits fixed-size packets (max 255 bytes FLRC payload).
 * This module provides the coalescing/fragmentation logic.
 *
 * ## TX Path (byte stream → packets)
 * TxFramer accumulates bytes from successive send() calls into an internal
 * buffer. When the buffer reaches MAX_PACKET bytes, it produces a full packet.
 * Remaining bytes stay buffered until the next call or flush.
 *
 * ## RX Path (packets → byte stream)
 * RxFramer holds a ring of received packet bytes. Each recv() call drains up
 * to buf_len bytes from the buffer. When the buffer is empty, the caller must
 * supply a new packet via push_packet().
 *
 * ## Wire Format
 * Each FLRC packet carries raw bytes from the Transport stream — no additional
 * framing header is added at this layer. The upper-layer FrameWriter already
 * wraps payloads with a 2-byte LE length prefix, so the stream is self-delimiting.
 * We just chunk it into radio-sized pieces.
 *
 * Ported per ADR-024 extract operation from microfips reference repo.
 */

#ifndef LR2021_FRAMING_H
#define LR2021_FRAMING_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <vector>

#ifdef __cplusplus

/// Maximum FLRC payload per packet (proven baseline: 255 bytes at 2600 kbps)
static constexpr size_t LR2021_FRAMING_MAX_PACKET = 255;

/**
 * TX-side framer: accumulates bytes, produces MAX_PACKET-sized chunks.
 *
 * Ported from Rust TxFramer struct.
 * Uses std::vector<uint8_t> instead of Rust heapless::Vec<u8, MAX_PACKET>.
 */
class TxFramer {
public:
    TxFramer() = default;

    /**
     * Push bytes into the framer. Returns number of bytes consumed from data.
     * If buffer fills to MAX_PACKET, exactly MAX_PACKET bytes are consumed
     * and the caller should call take_packet() to extract the full packet,
     * then call push() again for remaining bytes.
     *
     * @param data Pointer to input bytes
     * @param len  Number of bytes available
     * @return Number of bytes consumed (0 to min(len, space_remaining))
     */
    size_t push(const uint8_t* data, size_t len) {
        size_t space = LR2021_FRAMING_MAX_PACKET - buf_.size();
        size_t take = len < space ? len : space;
        buf_.insert(buf_.end(), data, data + take);
        return take;
    }

    /// Returns true if buffer is full (MAX_PACKET bytes ready to transmit).
    bool is_full() const {
        return buf_.size() >= LR2021_FRAMING_MAX_PACKET;
    }

    /**
     * Take the buffered bytes as an owned packet, clearing the buffer.
     * Returns the number of bytes written to out, or 0 if buffer was empty.
     *
     * @param out     Output buffer (must be at least MAX_PACKET bytes)
     * @param out_len Size of output buffer
     * @return Number of bytes written, or 0 if nothing to take
     */
    size_t take_packet(uint8_t* out, size_t out_len) {
        if (buf_.empty()) return 0;
        size_t n = buf_.size() < out_len ? buf_.size() : out_len;
        memcpy(out, buf_.data(), n);
        buf_.clear();
        return n;
    }

    /**
     * Convenience: take packet into a vector.
     * Returns empty vector if nothing buffered.
     */
    std::vector<uint8_t> take_packet_vec() {
        std::vector<uint8_t> result;
        result.swap(buf_);
        return result;
    }

    /// Clear the TX buffer after a packet has been transmitted.
    void clear() { buf_.clear(); }

    /// Number of bytes currently buffered.
    size_t pending() const { return buf_.size(); }

    /// Check if buffer has data waiting to be flushed.
    bool has_pending() const { return !buf_.empty(); }

private:
    std::vector<uint8_t> buf_;
};

/**
 * RX-side framer: buffers received packet bytes, drains on demand.
 *
 * Ported from Rust RxFramer struct.
 * Uses std::vector<uint8_t> with a soft capacity of MAX_PACKET * 2.
 */
class RxFramer {
public:
    RxFramer() : read_pos_(0) {}

    /**
     * Push a received packet's bytes into the framer.
     * Returns false if the buffer would overflow (packet dropped).
     *
     * @param data Pointer to packet bytes
     * @param len  Number of bytes in packet
     * @return true if accepted, false if buffer would overflow
     */
    bool push_packet(const uint8_t* data, size_t len) {
        // Compact if read_pos has advanced
        if (read_pos_ > 0) {
            compact();
        }
        // Check capacity (soft limit: MAX_PACKET * 2)
        constexpr size_t CAPACITY = LR2021_FRAMING_MAX_PACKET * 2;
        if (buf_.size() + len > CAPACITY) {
            return false;
        }
        buf_.insert(buf_.end(), data, data + len);
        return true;
    }

    /**
     * Drain up to buf_len bytes into buf. Returns number of bytes copied.
     * Returns 0 if no bytes are available (caller should wait for next packet).
     *
     * @param buf     Output buffer
     * @param buf_len Size of output buffer
     * @return Number of bytes copied
     */
    size_t drain(uint8_t* buf, size_t buf_len) {
        size_t available_bytes = available();
        if (available_bytes == 0) return 0;
        size_t n = buf_len < available_bytes ? buf_len : available_bytes;
        memcpy(buf, buf_.data() + read_pos_, n);
        read_pos_ += n;

        // Compact if fully drained
        if (read_pos_ >= buf_.size()) {
            buf_.clear();
            read_pos_ = 0;
        }
        return n;
    }

    /**
     * Convenience: drain into a vector.
     * Returns up to max_bytes bytes.
     */
    std::vector<uint8_t> drain_vec(size_t max_bytes = LR2021_FRAMING_MAX_PACKET * 2) {
        std::vector<uint8_t> result;
        size_t avail = available();
        size_t n = max_bytes < avail ? max_bytes : avail;
        result.assign(buf_.data() + read_pos_, buf_.data() + read_pos_ + n);
        read_pos_ += n;
        if (read_pos_ >= buf_.size()) {
            buf_.clear();
            read_pos_ = 0;
        }
        return result;
    }

    /// Number of bytes available to drain.
    size_t available() const {
        return buf_.size() - read_pos_;
    }

private:
    void compact() {
        size_t remaining = buf_.size() - read_pos_;
        if (remaining > 0) {
            memmove(buf_.data(), buf_.data() + read_pos_, remaining);
        }
        buf_.resize(remaining);
        read_pos_ = 0;
    }

    std::vector<uint8_t> buf_;
    size_t read_pos_;
};

#endif // __cplusplus
#endif // LR2021_FRAMING_H
