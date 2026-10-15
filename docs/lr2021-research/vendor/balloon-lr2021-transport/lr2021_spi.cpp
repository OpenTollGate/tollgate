/**
 * @file lr2021_spi.cpp
 * @brief LR2021 SPI driver implementation — MockLr2021Radio + EspIdfLr2021Radio.
 *
 * Ported from Rust microfips-esp-transport: lr2021_spi.rs (mock) + lr2021_esp_hal.rs (HAL).
 *
 * The MockLr2021Radio implementation is always compiled (host-testable).
 * The EspIdfLr2021Radio implementation is only compiled when ESP-IDF headers are present.
 *
 * Ported per ADR-024 extract operation from microfips reference repo.
 */

#include "lr2021_spi.h"

// ═══════════════════════════════════════════════════════════════════
// MockLr2021Radio implementation
// ═══════════════════════════════════════════════════════════════════

Lr2021Error MockLr2021Radio::send_packet(const uint8_t* data, size_t len) {
    tx_packets_.push_back(std::vector<uint8_t>(data, data + len));
    // Simulate TX_DONE
    irq_flags_ |= IrqSource::TX_DONE;
    return Lr2021Error::Ok;
}

Lr2021Error MockLr2021Radio::read_packet(uint8_t* buf, size_t buf_len, PacketStatus& status) {
    if (rx_queue_.empty()) {
        status = PacketStatus{};
        return Lr2021Error::Ok;
    }

    auto& pkt = rx_queue_.front();
    size_t n = pkt.size() < buf_len ? pkt.size() : buf_len;
    memcpy(buf, pkt.data(), n);
    rx_queue_.erase(rx_queue_.begin());

    status = PacketStatus{};
    status.length = n;
    status.crc_ok = true;
    return Lr2021Error::Ok;
}

Lr2021Error MockLr2021Radio::get_irq_status(uint32_t& flags) {
    flags = irq_flags_;
    return Lr2021Error::Ok;
}

Lr2021Error MockLr2021Radio::clear_irq() {
    irq_flags_ = 0;
    return Lr2021Error::Ok;
}

Lr2021Error MockLr2021Radio::check_irq(bool& asserted) {
    asserted = !IrqSource::empty(irq_flags_);
    return Lr2021Error::Ok;
}

// ═══════════════════════════════════════════════════════════════════
// EspIdfLr2021Radio implementation (real hardware, ESP-IDF only)
// ═══════════════════════════════════════════════════════════════════

#if defined(__has_include)
#if __has_include(<driver/spi_master.h>) && __has_include(<driver/gpio.h>) && __has_include(<esp_err.h>)

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "lr2021_spi";

EspIdfLr2021Radio::EspIdfLr2021Radio()
    : spi_dev_(nullptr)
    , spi_initialized_(false)
    , is_tx_(false)
{}

EspIdfLr2021Radio::~EspIdfLr2021Radio() {
    if (spi_initialized_ && spi_dev_ != nullptr) {
        spi_bus_remove_device(spi_dev_);
    }
}

esp_err_t EspIdfLr2021Radio::init_hardware() {
    // Initialize SPI bus
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num    = LR2021_PIN_MOSI;
    buscfg.miso_io_num    = LR2021_PIN_MISO;
    buscfg.sclk_io_num    = LR2021_PIN_SCLK;
    buscfg.quadwp_io_num  = -1;
    buscfg.quadhd_io_num  = -1;
    buscfg.max_transfer_sz = LR2021_MAX_PACKET + 8;

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init SPI bus: %s", esp_err_to_name(ret));
        return ret;
    }

    // Add device on bus
    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = LR2021_SPI_FREQ_HZ;
    devcfg.mode           = 0;  // SPI mode 0 (CPOL=0, CPHA=0)
    devcfg.spics_io_num   = LR2021_PIN_CS;
    devcfg.queue_size     = 1;
    devcfg.flags          = SPI_DEVICE_HALFDUPLEX;

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_dev_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure GPIO pins
    gpio_config_t io_conf = {};

    // BUSY pin: input
    io_conf.pin_bit_mask = (1ULL << LR2021_PIN_BUSY);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    // IRQ pin: input
    io_conf.pin_bit_mask = (1ULL << LR2021_PIN_IRQ);
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);

    // RST pin: output, default HIGH
    io_conf.pin_bit_mask = (1ULL << LR2021_PIN_RST);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    gpio_set_level((gpio_num_t)LR2021_PIN_RST, 1);

    spi_initialized_ = true;
    return ESP_OK;
}

// ── SPI helper methods ─────────────────────────────────────────────

Lr2021Error EspIdfLr2021Radio::spi_write(const uint8_t* data, size_t len) {
    if (!spi_initialized_) return Lr2021Error::WrongState;

    spi_transaction_t t = {};
    t.length = len * 8;
    t.tx_buffer = data;
    t.flags = 0;

    esp_err_t ret = spi_device_polling_transmit(spi_dev_, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI write failed: %s", esp_err_to_name(ret));
        return Lr2021Error::SpiError;
    }
    return Lr2021Error::Ok;
}

Lr2021Error EspIdfLr2021Radio::spi_read(const uint8_t* opcode, size_t opcode_len,
                                         uint8_t* buf, size_t buf_len) {
    if (!spi_initialized_) return Lr2021Error::WrongState;

    // Write opcode
    spi_transaction_t t = {};
    t.length = opcode_len * 8;
    t.tx_buffer = opcode;
    t.flags = 0;
    esp_err_t ret = spi_device_polling_transmit(spi_dev_, &t);
    if (ret != ESP_OK) return Lr2021Error::SpiError;

    // Wait for BUSY
    Lr2021Error err = wait_busy();
    if (err != Lr2021Error::Ok) return err;

    // Read data
    memset(buf, 0, buf_len);
    t = {};
    t.length = buf_len * 8;
    t.rx_buffer = buf;
    t.flags = 0;
    ret = spi_device_polling_transmit(spi_dev_, &t);
    if (ret != ESP_OK) return Lr2021Error::SpiError;

    return Lr2021Error::Ok;
}

Lr2021Error EspIdfLr2021Radio::wait_busy() {
    for (uint32_t i = 0; i < LR2021_BUSY_TIMEOUT_ITER; i++) {
        if (gpio_get_level((gpio_num_t)LR2021_PIN_BUSY) == 0) {
            return Lr2021Error::Ok;
        }
        // Small delay — on ESP-IDF we use ets_delay_us for tight polling
        // Using vTaskDelay(1) would be too coarse (1 tick = 10ms min)
        esp_rom_delay_us(1);
    }
    return Lr2021Error::Timeout;
}

bool EspIdfLr2021Radio::check_irq_pin() {
    return gpio_get_level((gpio_num_t)LR2021_PIN_IRQ) == 1;
}

Lr2021Error EspIdfLr2021Radio::hardware_reset() {
    // RST LOW 200us → HIGH, delay 50ms
    gpio_set_level((gpio_num_t)LR2021_PIN_RST, 0);
    esp_rom_delay_us(200);
    gpio_set_level((gpio_num_t)LR2021_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    return Lr2021Error::Ok;
}

// ── Command helper ─────────────────────────────────────────────────

Lr2021Error EspIdfLr2021Radio::cmd(const uint8_t* data, size_t len) {
    Lr2021Error err = wait_busy();
    if (err != Lr2021Error::Ok) return err;
    return spi_write(data, len);
}

// ── Config helpers (ported from lr2021_esp_hal.rs) ─────────────────

void EspIdfLr2021Radio::compute_frf(float freq_mhz, uint8_t out[3]) {
    // frf = (freq_MHz * 1e6 * 2^18) / (XTAL_MHz * 1e6)
    uint32_t frf = (uint32_t)((freq_mhz * 1e6f * 262144.0f) / (LR2021_XTAL_MHZ * 1e6f));
    // 2^18 = 262144
    out[0] = (frf >> 16) & 0xFF;
    out[1] = (frf >> 8) & 0xFF;
    out[2] = frf & 0xFF;
}

uint8_t EspIdfLr2021Radio::bitrate_to_brbw(uint32_t bitrate_kbps) {
    switch (bitrate_kbps) {
        case 2600: return 0x00;
        case 2080: return 0x01;
        case 1300: return 0x02;
        case 650:  return 0x03;
        case 325:  return 0x04;
        default:   return 0x00; // default to max
    }
}

/**
 * Build FLRC packet params bytes.
 * preamble=16 (index 3), syncTx=1, syncMatch=1, fixed=1, crc depends on config.
 */
static void build_packet_params(const Lr2021Config& config, uint8_t params[4]) {
    // Byte 0: ((preambleIndex & 0x0F) << 2) | (syncWordLen / 2)
    // preamble=16 → index 3: (3<<2)|2 = 0x0E
    uint8_t byte0 = 0x0E;
    // Byte 1: ((syncTx & 0x03) << 6) | ((syncMatch & 0x07) << 3) | (fixedLen<<2) | crc
    uint8_t crc_byte = config.crc_enabled ? 0x01 : 0x00;
    uint8_t byte1 = (1u << 6) | (1u << 3) | (1u << 2) | crc_byte;
    // Byte 2-3: payloadLen big-endian (upper byte 0 for ≤255)
    params[0] = byte0;
    params[1] = byte1;
    params[2] = 0x00;
    params[3] = config.payload_length;
}

Lr2021Error EspIdfLr2021Radio::init_sequence(const Lr2021Config& config) {
    // Step 0: Hardware reset
    Lr2021Error err = hardware_reset();
    if (err != Lr2021Error::Ok) return err;

    // Step 1: CLEAR_ERRORS
    err = cmd(Lr2021Opcodes::OP_CLEAR_ERRORS, sizeof(Lr2021Opcodes::OP_CLEAR_ERRORS));
    if (err != Lr2021Error::Ok) return err;

    // Step 2: SET_STANDBY (XOSC)
    err = cmd(Lr2021Opcodes::OP_SET_STANDBY_XOSC, sizeof(Lr2021Opcodes::OP_SET_STANDBY_XOSC));
    if (err != Lr2021Error::Ok) return err;

    // Step 3: SET_PACKET_TYPE FLRC
    err = cmd(Lr2021Opcodes::OP_SET_PACKET_TYPE_FLRC, sizeof(Lr2021Opcodes::OP_SET_PACKET_TYPE_FLRC));
    if (err != Lr2021Error::Ok) return err;

    // Step 4: SET_RF_FREQUENCY
    uint8_t frf[3];
    compute_frf(config.freq_mhz, frf);
    uint8_t freq_cmd[5] = {
        Lr2021Opcodes::OP_SET_RF_FREQUENCY[0],
        Lr2021Opcodes::OP_SET_RF_FREQUENCY[1],
        frf[0], frf[1], frf[2]
    };
    err = cmd(freq_cmd, 5);
    if (err != Lr2021Error::Ok) return err;

    // Step 5: SET_RX_PATH (HF)
    err = cmd(Lr2021Opcodes::OP_SET_RX_PATH_HF, sizeof(Lr2021Opcodes::OP_SET_RX_PATH_HF));
    if (err != Lr2021Error::Ok) return err;

    // Step 6: CALIB_FRONT_END (freq/4 | 0x8000 + padding)
    uint16_t freq_div4 = (uint16_t)((config.freq_mhz / 4.0f)) | 0x8000;
    uint8_t cal_fe[6] = {
        Lr2021Opcodes::OP_CALIB_FRONT_END[0],
        Lr2021Opcodes::OP_CALIB_FRONT_END[1],
        (uint8_t)(freq_div4 >> 8),
        (uint8_t)(freq_div4 & 0xFF),
        0x00, 0x00
    };
    err = cmd(cal_fe, 6);
    if (err != Lr2021Error::Ok) return err;

    // Step 7: CALIBRATE (all blocks, 0x5F)
    err = cmd(Lr2021Opcodes::OP_CALIBRATE_ALL, sizeof(Lr2021Opcodes::OP_CALIBRATE_ALL));
    if (err != Lr2021Error::Ok) return err;
    err = wait_busy();
    if (err != Lr2021Error::Ok) return err;

    // Step 8: SET_FLRC_MOD_PARAMS
    uint8_t brbw = bitrate_to_brbw(config.bitrate_kbps);
    uint8_t mod_params[4] = {
        Lr2021Opcodes::OP_SET_FLRC_MOD_PARAMS[0],
        Lr2021Opcodes::OP_SET_FLRC_MOD_PARAMS[1],
        brbw,
        0x25 // CR_1_0, BT0.5
    };
    err = cmd(mod_params, 4);
    if (err != Lr2021Error::Ok) return err;

    // Step 9: SET_FLRC_SYNCWORD
    uint8_t sync_cmd[7] = {
        Lr2021Opcodes::OP_SET_FLRC_SYNCWORD[0],
        Lr2021Opcodes::OP_SET_FLRC_SYNCWORD[1],
        0x01, // syncWordLen = 4 bytes (type 1)
        config.sync_word[0],
        config.sync_word[1],
        config.sync_word[2],
        config.sync_word[3]
    };
    err = cmd(sync_cmd, 7);
    if (err != Lr2021Error::Ok) return err;

    // Step 10: SET_FLRC_PACKET_PARAMS
    uint8_t pkt_params[4];
    build_packet_params(config, pkt_params);
    uint8_t pp_cmd[6] = {
        Lr2021Opcodes::OP_SET_FLRC_PACKET_PARAMS[0],
        Lr2021Opcodes::OP_SET_FLRC_PACKET_PARAMS[1],
        pkt_params[0], pkt_params[1], pkt_params[2], pkt_params[3]
    };
    err = cmd(pp_cmd, 6);
    if (err != Lr2021Error::Ok) return err;

    // TX-only: SET_PA_CONFIG + SET_TX_PARAMS (between step 10 and 11)
    err = cmd(Lr2021Opcodes::OP_SET_PA_CONFIG, sizeof(Lr2021Opcodes::OP_SET_PA_CONFIG));
    if (err != Lr2021Error::Ok) return err;

    uint8_t power_raw = (uint8_t)(config.tx_power_dbm * 2.0f + 0.5f);
    uint8_t txp_cmd[4] = {
        Lr2021Opcodes::OP_SET_TX_PARAMS[0],
        Lr2021Opcodes::OP_SET_TX_PARAMS[1],
        power_raw,
        0x04 // ramp time
    };
    err = cmd(txp_cmd, 4);
    if (err != Lr2021Error::Ok) return err;

    // Step 11: SET_RX_TX_FALLBACK = Fs (0x03) — keeps PLL warm
    err = cmd(Lr2021Opcodes::OP_SET_RX_TX_FALLBACK_FS, sizeof(Lr2021Opcodes::OP_SET_RX_TX_FALLBACK_FS));
    if (err != Lr2021Error::Ok) return err;

    // Step 12: DIO_FUNCTION (DIO9 = IRQ)
    err = cmd(Lr2021Opcodes::OP_DIO_FUNCTION, sizeof(Lr2021Opcodes::OP_DIO_FUNCTION));
    if (err != Lr2021Error::Ok) return err;

    // Step 13: DIO_IRQ_CONFIG — map both RX_DONE and TX_DONE
    err = cmd(Lr2021Opcodes::OP_DIO_IRQ_CONFIG_RX, sizeof(Lr2021Opcodes::OP_DIO_IRQ_CONFIG_RX));
    if (err != Lr2021Error::Ok) return err;
    err = cmd(Lr2021Opcodes::OP_DIO_IRQ_CONFIG_TX, sizeof(Lr2021Opcodes::OP_DIO_IRQ_CONFIG_TX));
    if (err != Lr2021Error::Ok) return err;

    // Step 14: CLEAR_IRQ
    err = cmd(Lr2021Opcodes::OP_CLEAR_IRQ, sizeof(Lr2021Opcodes::OP_CLEAR_IRQ));
    if (err != Lr2021Error::Ok) return err;

    return Lr2021Error::Ok;
}

// ── Lr2021Radio interface implementation ───────────────────────────

Lr2021Error EspIdfLr2021Radio::init(const Lr2021Config& config) {
    if (!spi_initialized_) {
        if (init_hardware() != ESP_OK) return Lr2021Error::SpiError;
    }

    Lr2021Error err = init_sequence(config);
    if (err != Lr2021Error::Ok) return err;

    // Enter RX mode after init
    return start_rx();
}

Lr2021Error EspIdfLr2021Radio::start_rx() {
    is_tx_ = false;
    // Clear RX FIFO before entering RX
    Lr2021Error err = cmd(Lr2021Opcodes::OP_CLR_RX_FIFO, sizeof(Lr2021Opcodes::OP_CLR_RX_FIFO));
    if (err != Lr2021Error::Ok) return err;
    err = cmd(Lr2021Opcodes::OP_CLEAR_IRQ, sizeof(Lr2021Opcodes::OP_CLEAR_IRQ));
    if (err != Lr2021Error::Ok) return err;
    // SET_RX continuous (5 bytes!)
    return cmd(Lr2021Opcodes::OP_SET_RX_CONTINUOUS, sizeof(Lr2021Opcodes::OP_SET_RX_CONTINUOUS));
}

Lr2021Error EspIdfLr2021Radio::send_packet(const uint8_t* data, size_t len) {
    if (len > LR2021_MAX_PACKET) return Lr2021Error::PacketTooLong;

    is_tx_ = true;

    // Clear TX FIFO (MANDATORY — stale bytes corrupt sync word)
    Lr2021Error err = cmd(Lr2021Opcodes::OP_CLR_TX_FIFO, sizeof(Lr2021Opcodes::OP_CLR_TX_FIFO));
    if (err != Lr2021Error::Ok) return err;
    // Clear IRQ
    err = cmd(Lr2021Opcodes::OP_CLEAR_IRQ, sizeof(Lr2021Opcodes::OP_CLEAR_IRQ));
    if (err != Lr2021Error::Ok) return err;

    // Write TX FIFO: opcode + length byte + payload
    // Batch into single SPI transaction for speed (2.44x faster per discovery)
    std::vector<uint8_t> buf;
    buf.reserve(len + 3);
    buf.insert(buf.end(), Lr2021Opcodes::OP_WRITE_TX_FIFO,
               Lr2021Opcodes::OP_WRITE_TX_FIFO + 2);
    buf.push_back((uint8_t)len);
    buf.insert(buf.end(), data, data + len);
    err = cmd(buf.data(), buf.size());
    if (err != Lr2021Error::Ok) return err;

    // SET_TX (5 bytes!)
    return cmd(Lr2021Opcodes::OP_SET_TX_CMD, sizeof(Lr2021Opcodes::OP_SET_TX_CMD));
}

Lr2021Error EspIdfLr2021Radio::read_packet(uint8_t* buf, size_t buf_len, PacketStatus& status) {
    // Get RX buffer status: returns [status, length, offset]
    uint8_t status_buf[3] = {};
    Lr2021Error err = spi_read(Lr2021Opcodes::OP_GET_RX_BUFFER_STATUS, 2, status_buf, 3);
    if (err != Lr2021Error::Ok) return err;

    size_t length = status_buf[1];
    size_t offset = status_buf[2];

    if (length == 0) {
        status = PacketStatus{};
        return Lr2021Error::Ok;
    }

    size_t n = length < buf_len ? length : buf_len;

    // Read RX FIFO at offset
    uint8_t read_fifo_cmd[3] = {
        Lr2021Opcodes::OP_READ_RX_FIFO[0],
        Lr2021Opcodes::OP_READ_RX_FIFO[1],
        (uint8_t)offset
    };
    err = spi_read(read_fifo_cmd, 3, buf, n);
    if (err != Lr2021Error::Ok) return err;

    // Get packet status (RSSI, SNR)
    uint8_t pkt_status[4] = {};
    err = spi_read(Lr2021Opcodes::OP_GET_PACKET_STATUS, 2, pkt_status, 4);
    if (err != Lr2021Error::Ok) return err;

    int8_t rssi_raw = (int8_t)pkt_status[1];

    status = PacketStatus{};
    status.length = n;
    status.rssi_dbm = (int16_t)rssi_raw;
    status.snr_db = 0;
    status.crc_ok = true; // checked via IRQ CRC_ERROR flag
    return Lr2021Error::Ok;
}

Lr2021Error EspIdfLr2021Radio::get_irq_status(uint32_t& flags) {
    // Read 4 bytes of IRQ status (32-bit)
    uint8_t irq_buf[4] = {};
    Lr2021Error err = spi_read(Lr2021Opcodes::OP_GET_IRQ_STATUS, 2, irq_buf, 4);
    if (err != Lr2021Error::Ok) return err;

    // Big-endian u32
    flags = ((uint32_t)irq_buf[0] << 24) |
            ((uint32_t)irq_buf[1] << 16) |
            ((uint32_t)irq_buf[2] << 8)  |
            ((uint32_t)irq_buf[3]);
    return Lr2021Error::Ok;
}

Lr2021Error EspIdfLr2021Radio::clear_irq() {
    return cmd(Lr2021Opcodes::OP_CLEAR_IRQ, sizeof(Lr2021Opcodes::OP_CLEAR_IRQ));
}

Lr2021Error EspIdfLr2021Radio::check_irq(bool& asserted) {
    asserted = check_irq_pin();
    return Lr2021Error::Ok;
}

Lr2021Error EspIdfLr2021Radio::standby() {
    return cmd(Lr2021Opcodes::OP_SET_STANDBY_RC, sizeof(Lr2021Opcodes::OP_SET_STANDBY_RC));
}

Lr2021Error EspIdfLr2021Radio::sleep() {
    return cmd(Lr2021Opcodes::OP_SET_SLEEP, sizeof(Lr2021Opcodes::OP_SET_SLEEP));
}

#endif // __has_include(<driver/...>)
#endif // defined(__has_include)
