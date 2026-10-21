/**
 * @file lr2021_spi.h
 * @brief LR2021 SPI driver — register-level communication with the Semtech LR2021 radio.
 *
 * Ported from Rust microfips-esp-transport: lr2021_spi.rs + lr2021_esp_hal.rs
 *
 * Provides:
 * 1. Abstract Lr2021Radio interface (decouples transport from SPI/GPIO implementation)
 * 2. MockLr2021Radio for unit testing (no hardware required)
 * 3. EspIdfLr2021Radio for ESP32-C3 with ESP-IDF (real hardware, conditional on ESP-IDF)
 *
 * ## LR2021 SPI Protocol Summary
 *
 * The LR2021 uses a half-duplex SPI interface with a BUSY handshake:
 * 1. Wait for BUSY pin LOW
 * 2. Assert CS LOW
 * 3. Send command/address byte (bit 7: R/W, bits 6-0: address)
 * 4. Send/receive data bytes
 * 5. Deassert CS HIGH
 *
 * Commands use 2-byte opcodes (Semtech LR2021 datasheet).
 * The radio handles preamble/sync/CRC in hardware. We only provide and receive payload bytes.
 *
 * Ported per ADR-024 extract operation from microfips reference repo.
 */

#ifndef LR2021_SPI_H
#define LR2021_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <vector>
#include <optional>

#ifdef __cplusplus

// ── ESP32-C3 Pin Definitions ───────────────────────────────────────
// From hardware verification in lr2021_esp_hal.rs:
//   GPIO7  = SPI MOSI
//   GPIO2  = SPI MISO
//   GPIO6  = SPI SCLK
//   GPIO10 = SPI CS (NSS)
//   GPIO3  = LR2021 RESET
//   GPIO4  = LR2021 BUSY
//   GPIO5  = LR2021 DIO9 (IRQ)

#define LR2021_PIN_MOSI    7    // GPIO7  — SPI MOSI
#define LR2021_PIN_MISO    2    // GPIO2  — SPI MISO
#define LR2021_PIN_SCLK    6    // GPIO6  — SPI SCLK
#define LR2021_PIN_CS      10   // GPIO10 — SPI CS (NSS)
#define LR2021_PIN_RST     3    // GPIO3  — LR2021 RESET
#define LR2021_PIN_BUSY    4    // GPIO4  — LR2021 BUSY
#define LR2021_PIN_IRQ     5    // GPIO5  — LR2021 DIO9 (IRQ)

// ── Hardware Constants ─────────────────────────────────────────────

/// LR2021 module crystal frequency in MHz
#define LR2021_XTAL_MHZ       52.0f

/// SPI clock frequency (20 MHz max verified per speed-tests)
#define LR2021_SPI_FREQ_HZ    20000000U

/// Maximum FLRC payload per packet (proven baseline: 255 bytes at 2600 kbps)
#define LR2021_MAX_PACKET     255u

/// Busy pin poll timeout (iterations)
#define LR2021_BUSY_TIMEOUT_ITER  100000U

/// TX done poll timeout (iterations)
#define LR2021_TX_DONE_ITER       100000U

// ── Error Type ─────────────────────────────────────────────────────

/// Error codes mirroring Rust Lr2021Error enum.
/// Ok = success (Rust returns Result<(), Lr2021Error>, C++ uses this value).
enum class Lr2021Error {
    Ok,             ///< Success (equivalent to Rust Ok(()))
    SpiError,       ///< SPI bus error (transfer failed)
    Timeout,        ///< Radio not responding (BUSY stuck HIGH, no IRQ)
    InvalidConfig,  ///< Invalid configuration parameter
    CrcMismatch,    ///< CRC check failed on received packet
    PacketTooLong,  ///< Packet larger than MAX_PACKET
    WrongState,     ///< Radio in wrong state for requested operation
};

/// Convenience: check if an Lr2021Error represents success
inline bool lr2021_ok(Lr2021Error e) { return e == Lr2021Error::Ok; }

// ── Configuration ──────────────────────────────────────────────────

/// FLRC modulation parameters (from proven balloon baseline)
struct Lr2021Config {
    float    freq_mhz;        ///< Operating frequency in MHz (e.g., 2440.0 for 2.4 GHz ISM)
    uint32_t bitrate_kbps;    ///< FLRC bitrate in kbps (supported: 2600, 1300, 650, 325)
    int8_t   tx_power_dbm;    ///< TX power in dBm (range: -18 to +12)
    uint8_t  sync_word[4];    ///< Sync word bytes (must match on TX and RX)
    bool     crc_enabled;     ///< Enable CRC (recommended)
    uint8_t  payload_length;  ///< Maximum payload length per packet (FLRC: max 255)

    /// Default config — proven baseline from balloon-range-tests Track 1
    Lr2021Config()
        : freq_mhz(2440.0f)
        , bitrate_kbps(2600)
        , tx_power_dbm(12)
        , crc_enabled(true)
        , payload_length(255)
    {
        sync_word[0] = 0x12;
        sync_word[1] = 0xAD;
        sync_word[2] = 0x10;
        sync_word[3] = 0x1B;
    }
};

// ── IRQ Source Flags ───────────────────────────────────────────────
// 32-bit flags from the LR2021's 4-byte IRQ status register.
// Mirrors Rust bitflags! IrqSource struct.

namespace IrqSource {
    constexpr uint32_t TX_DONE           = 0x00080000u; ///< Bit 19
    constexpr uint32_t RX_DONE           = 0x00040000u; ///< Bit 18
    constexpr uint32_t CMD_ERROR         = 0x00020000u; ///< Bit 17
    constexpr uint32_t CRC_ERROR         = 0x00100000u; ///< Bit 20
    constexpr uint32_t TIMEOUT           = 0x00200000u; ///< Bit 21
    constexpr uint32_t PREAMBLE_DETECTED = 0x00000002u; ///< Bit 1
    constexpr uint32_t SYNCWORD_VALID    = 0x00000004u; ///< Bit 2
    constexpr uint32_t HEADER_VALID      = 0x00000008u; ///< Bit 3
    constexpr uint32_t ALL               = 0xFFFFFFFFu;

    /// Check if flag is set in mask
    inline bool contains(uint32_t mask, uint32_t flag) { return (mask & flag) != 0; }
    /// Check if no flags set
    inline bool empty(uint32_t mask) { return mask == 0; }
}

// ── Packet Status ──────────────────────────────────────────────────

/// Status of a received packet
struct PacketStatus {
    size_t length;     ///< Number of payload bytes received
    int16_t rssi_dbm;  ///< RSSI of the received packet (dBm, negative)
    int8_t  snr_db;    ///< SNR of the received packet (dB)
    bool    crc_ok;    ///< CRC passed

    PacketStatus()
        : length(0), rssi_dbm(-127), snr_db(-127), crc_ok(true) {}
};

// NOTE: SX1280 1-byte opcodes (Lr2021Commands) and 16-bit register addresses
// (Lr2021Registers) removed — incompatible with LR2021 2-byte protocol per ADR-020.

// ── 2-Byte SPI Opcodes (from lr2021_esp_hal.rs) ────────────────────
// Raw 2-byte command opcodes verified on RP2040/ESP32-C3.

namespace Lr2021Opcodes {
    // Write/command opcodes
    static const uint8_t OP_CLEAR_ERRORS[]         = {0x01, 0x11, 0x00, 0x00};
    static const uint8_t OP_SET_STANDBY_XOSC[]     = {0x01, 0x28, 0x01};
    static const uint8_t OP_SET_PACKET_TYPE_FLRC[] = {0x02, 0x07, 0x05};
    static const uint8_t OP_SET_RF_FREQUENCY[]     = {0x02, 0x00};
    static const uint8_t OP_SET_RX_PATH_HF[]       = {0x02, 0x01, 0x01, 0x00};
    static const uint8_t OP_CALIB_FRONT_END[]      = {0x01, 0x23};
    static const uint8_t OP_CALIBRATE_ALL[]        = {0x01, 0x22, 0x5F}; ///< 0x5F not 0x6F
    static const uint8_t OP_SET_FLRC_MOD_PARAMS[]  = {0x02, 0x48, 0x00, 0x25}; ///< 2600kbps, CR_1_0, BT0.5
    static const uint8_t OP_SET_FLRC_SYNCWORD[]    = {0x02, 0x4C};
    static const uint8_t OP_SET_FLRC_PACKET_PARAMS[] = {0x02, 0x49};
    static const uint8_t OP_SET_RX_TX_FALLBACK_FS[]  = {0x02, 0x06, 0x03}; ///< Fs=0x03, NOT STDBY_RC=0x00
    static const uint8_t OP_DIO_FUNCTION[]         = {0x01, 0x12, 0x09, 0x11}; ///< DIO9 = IRQ
    static const uint8_t OP_CLEAR_IRQ[]            = {0x01, 0x16, 0xFF, 0xFF, 0xFF, 0xFF};
    static const uint8_t OP_SET_RX_CONTINUOUS[]    = {0x02, 0x0C, 0xFF, 0xFF, 0xFF}; ///< 5 bytes!
    static const uint8_t OP_SET_TX_CMD[]           = {0x02, 0x0D, 0x00, 0x00, 0x00}; ///< 5 bytes!
    static const uint8_t OP_SET_PA_CONFIG[]        = {0x02, 0x02, 0x80, 0x00, 0x60, 0x07, 0x10};
    static const uint8_t OP_SET_TX_PARAMS[]        = {0x02, 0x03};
    static const uint8_t OP_SET_STANDBY_RC[]       = {0x01, 0x28, 0x00};
    static const uint8_t OP_SET_SLEEP[]            = {0x01, 0x29, 0x00};

    // Read opcodes
    static const uint8_t OP_GET_IRQ_STATUS[]       = {0x01, 0x17};
    static const uint8_t OP_GET_AND_CLEAR_IRQ[]    = {0x01, 0x18};
    static const uint8_t OP_GET_RX_BUFFER_STATUS[] = {0x01, 0x13};
    static const uint8_t OP_GET_PACKET_STATUS[]    = {0x01, 0x14};

    // FIFO operations
    static const uint8_t OP_WRITE_TX_FIFO[]        = {0x00, 0x02};
    static const uint8_t OP_READ_RX_FIFO[]         = {0x00, 0x03};
    static const uint8_t OP_CLR_TX_FIFO[]          = {0x01, 0x1F};
    static const uint8_t OP_CLR_RX_FIFO[]          = {0x01, 0x1E};

    // DIO IRQ config — map RX_DONE (bit18) and TX_DONE (bit19)
    static const uint8_t OP_DIO_IRQ_CONFIG_RX[]    = {0x01, 0x15, 0x00, 0x04, 0x00, 0x00};
    static const uint8_t OP_DIO_IRQ_CONFIG_TX[]    = {0x01, 0x15, 0x00, 0x08, 0x00, 0x00};

    /// Helper: get array length (compile-time)
    template <size_t N>
    constexpr size_t arr_len(const uint8_t (&)[N]) { return N; }
}

// ── Abstract Radio Interface ───────────────────────────────────────

/**
 * Abstract LR2021 radio interface.
 *
 * C++ equivalent of the Rust `Lr2021Radio` trait.
 * Implemented by:
 * - MockLr2021Radio (unit tests, no hardware)
 * - EspIdfLr2021Radio (ESP32-C3 with ESP-IDF, real hardware)
 *
 * Methods are blocking (no async in ESP-IDF C++ transport layer).
 */
class Lr2021Radio {
public:
    virtual ~Lr2021Radio() = default;

    /// Initialize the radio with the given FLRC configuration.
    virtual Lr2021Error init(const Lr2021Config& config) = 0;

    /// Start receiving (enter RX mode).
    virtual Lr2021Error start_rx() = 0;

    /// Send a packet (enters TX mode, transmits).
    /// Packet data must be ≤ MAX_PACKET bytes.
    virtual Lr2021Error send_packet(const uint8_t* data, size_t len) = 0;

    /// Read a received packet from the radio's buffer.
    /// Returns packet status (length, RSSI, SNR, CRC).
    virtual Lr2021Error read_packet(uint8_t* buf, size_t buf_len, PacketStatus& status) = 0;

    /// Get the current IRQ status flags.
    virtual Lr2021Error get_irq_status(uint32_t& flags) = 0;

    /// Clear IRQ status flags (write 1 to clear).
    virtual Lr2021Error clear_irq() = 0;

    /// Check if the IRQ pin is asserted (for polling mode).
    /// Returns true if an interrupt is pending.
    virtual Lr2021Error check_irq(bool& asserted) = 0;

    /// Put the radio into standby mode (low power, quick wakeup).
    virtual Lr2021Error standby() = 0;

    /// Put the radio to sleep (lowest power, needs re-init on wake).
    virtual Lr2021Error sleep() = 0;
};

// ── Mock Implementation (for unit tests) ───────────────────────────

/**
 * Mock LR2021 radio for unit testing.
 *
 * Stores TX packets in a buffer that can be inspected by the test.
 * RX packets are pre-loaded via load_rx_packet().
 *
 * C++ equivalent of Rust MockLr2021Radio.
 */
class MockLr2021Radio : public Lr2021Radio {
public:
    MockLr2021Radio() : irq_flags_(0), initialized_(false) {}

    /// Pre-load a packet that will be returned on the next read_packet() call.
    /// Also sets RX_DONE IRQ flag.
    void load_rx_packet(const uint8_t* data, size_t len) {
        rx_queue_.push_back(std::vector<uint8_t>(data, data + len));
        irq_flags_ |= IrqSource::RX_DONE;
    }

    /// Get all packets that were transmitted (for test assertions).
    const std::vector<std::vector<uint8_t>>& get_tx_packets() const {
        return tx_packets_;
    }

    /// Get the last configuration passed to init().
    const std::optional<Lr2021Config>& get_config() const { return config_; }

    /// Check if init() was called.
    bool is_initialized() const { return initialized_; }

    // ── Lr2021Radio interface ──

    Lr2021Error init(const Lr2021Config& config) override {
        config_ = config;
        initialized_ = true;
        return Lr2021Error::Ok;
    }

    Lr2021Error start_rx() override { return Lr2021Error::Ok; }

    Lr2021Error send_packet(const uint8_t* data, size_t len) override;
    Lr2021Error read_packet(uint8_t* buf, size_t buf_len, PacketStatus& status) override;
    Lr2021Error get_irq_status(uint32_t& flags) override;
    Lr2021Error clear_irq() override;
    Lr2021Error check_irq(bool& asserted) override;
    Lr2021Error standby() override { return Lr2021Error::Ok; }
    Lr2021Error sleep() override { return Lr2021Error::Ok; }

private:
    std::vector<std::vector<uint8_t>> tx_packets_;
    std::vector<std::vector<uint8_t>> rx_queue_;
    uint32_t irq_flags_;
    bool initialized_;
    std::optional<Lr2021Config> config_;
};

// ── ESP-IDF Radio Implementation (real hardware) ────────────────────
// Only compiled when ESP-IDF headers are available (target build)

#if defined(__has_include)
#if __has_include(<driver/spi_master.h>) && __has_include(<driver/gpio.h>) && __has_include(<esp_err.h>)

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include "esp_log.h"

/**
 * Real LR2021 radio driver for ESP32-C3 using ESP-IDF.
 *
 * Uses ESP-IDF spi_device_handle_t + gpio_num_t for BUSY/IRQ/RST pins.
 * Blocking SPI + GPIO polling for BUSY/IRQ (simplest, proven on RP2040).
 *
 * Ported from Rust EspHalLr2021Radio.
 */
class EspIdfLr2021Radio : public Lr2021Radio {
public:
    EspIdfLr2021Radio();
    ~EspIdfLr2021Radio() override;

    /**
     * Initialize SPI bus and GPIO pins.
     * Call before init().
     * Uses the pin definitions from LR2021_PIN_* macros.
     */
    esp_err_t init_hardware();

    // ── Lr2021Radio interface ──

    Lr2021Error init(const Lr2021Config& config) override;
    Lr2021Error start_rx() override;
    Lr2021Error send_packet(const uint8_t* data, size_t len) override;
    Lr2021Error read_packet(uint8_t* buf, size_t buf_len, PacketStatus& status) override;
    Lr2021Error get_irq_status(uint32_t& flags) override;
    Lr2021Error clear_irq() override;
    Lr2021Error check_irq(bool& asserted) override;
    Lr2021Error standby() override;
    Lr2021Error sleep() override;

private:
    // SPI/GPIO handles
    spi_device_handle_t spi_dev_;
    bool spi_initialized_;
    bool is_tx_;

    // SPI helper methods
    Lr2021Error spi_write(const uint8_t* data, size_t len);
    Lr2021Error spi_read(const uint8_t* opcode, size_t opcode_len, uint8_t* buf, size_t buf_len);
    Lr2021Error wait_busy();
    bool check_irq_pin();
    Lr2021Error hardware_reset();

    // Command helper
    Lr2021Error cmd(const uint8_t* data, size_t len);

    // Config helpers
    static void compute_frf(float freq_mhz, uint8_t out[3]);
    static uint8_t bitrate_to_brbw(uint32_t bitrate_kbps);
    Lr2021Error init_sequence(const Lr2021Config& config);
};

#endif // __has_include
#endif // defined(__has_include)

#endif // __cplusplus
#endif // LR2021_SPI_H
