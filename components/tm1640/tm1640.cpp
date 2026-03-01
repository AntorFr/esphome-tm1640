#include "tm1640.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tm1640 {

static const char *const TAG = "display.tm1640";

// TM1640 Commands
const uint8_t TM1640_CMD_DATA = 0x40;  //!< Data command (write mode, auto-increment)
const uint8_t TM1640_CMD_CTRL = 0x80;  //!< Display control command
const uint8_t TM1640_CMD_ADDR = 0xC0;  //!< Address command

const uint8_t TM1640_UNKNOWN_CHAR = 0b11111111;

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D   X
// XABCDEFG
constexpr uint8_t TM1640_ASCII_TO_RAW[] PROGMEM = {
    0b00000000,           // ' ', ord 0x20
    0b10110000,           // '!', ord 0x21
    0b00100010,           // '"', ord 0x22
    TM1640_UNKNOWN_CHAR,  // '#', ord 0x23
    TM1640_UNKNOWN_CHAR,  // '$', ord 0x24
    0b01001001,           // '%', ord 0x25
    TM1640_UNKNOWN_CHAR,  // '&', ord 0x26
    0b00000010,           // ''', ord 0x27
    0b01001110,           // '(', ord 0x28
    0b01111000,           // ')', ord 0x29
    0b01000000,           // '*', ord 0x2A
    TM1640_UNKNOWN_CHAR,  // '+', ord 0x2B
    0b00010000,           // ',', ord 0x2C
    0b00000001,           // '-', ord 0x2D
    0b10000000,           // '.', ord 0x2E
    TM1640_UNKNOWN_CHAR,  // '/', ord 0x2F
    0b01111110,           // '0', ord 0x30
    0b00110000,           // '1', ord 0x31
    0b01101101,           // '2', ord 0x32
    0b01111001,           // '3', ord 0x33
    0b00110011,           // '4', ord 0x34
    0b01011011,           // '5', ord 0x35
    0b01011111,           // '6', ord 0x36
    0b01110000,           // '7', ord 0x37
    0b01111111,           // '8', ord 0x38
    0b01111011,           // '9', ord 0x39
    0b01001000,           // ':', ord 0x3A
    0b01011000,           // ';', ord 0x3B
    TM1640_UNKNOWN_CHAR,  // '<', ord 0x3C
    TM1640_UNKNOWN_CHAR,  // '=', ord 0x3D
    TM1640_UNKNOWN_CHAR,  // '>', ord 0x3E
    0b01100101,           // '?', ord 0x3F
    0b01101111,           // '@', ord 0x40
    0b01110111,           // 'A', ord 0x41
    0b00011111,           // 'B', ord 0x42
    0b01001110,           // 'C', ord 0x43
    0b00111101,           // 'D', ord 0x44
    0b01001111,           // 'E', ord 0x45
    0b01000111,           // 'F', ord 0x46
    0b01011110,           // 'G', ord 0x47
    0b00110111,           // 'H', ord 0x48
    0b00110000,           // 'I', ord 0x49
    0b00111100,           // 'J', ord 0x4A
    TM1640_UNKNOWN_CHAR,  // 'K', ord 0x4B
    0b00001110,           // 'L', ord 0x4C
    TM1640_UNKNOWN_CHAR,  // 'M', ord 0x4D
    0b00010101,           // 'N', ord 0x4E
    0b01111110,           // 'O', ord 0x4F
    0b01100111,           // 'P', ord 0x50
    0b11111110,           // 'Q', ord 0x51
    0b00000101,           // 'R', ord 0x52
    0b01011011,           // 'S', ord 0x53
    0b00000111,           // 'T', ord 0x54
    0b00111110,           // 'U', ord 0x55
    0b00111110,           // 'V', ord 0x56
    0b00111111,           // 'W', ord 0x57
    TM1640_UNKNOWN_CHAR,  // 'X', ord 0x58
    0b00100111,           // 'Y', ord 0x59
    0b01101101,           // 'Z', ord 0x5A
    0b01001110,           // '[', ord 0x5B
    TM1640_UNKNOWN_CHAR,  // '\', ord 0x5C
    0b01111000,           // ']', ord 0x5D
    TM1640_UNKNOWN_CHAR,  // '^', ord 0x5E
    0b00001000,           // '_', ord 0x5F
    0b00100000,           // '`', ord 0x60
    0b01110111,           // 'a', ord 0x61
    0b00011111,           // 'b', ord 0x62
    0b00001101,           // 'c', ord 0x63
    0b00111101,           // 'd', ord 0x64
    0b01001111,           // 'e', ord 0x65
    0b01000111,           // 'f', ord 0x66
    0b01011110,           // 'g', ord 0x67
    0b00010111,           // 'h', ord 0x68
    0b00010000,           // 'i', ord 0x69
    0b00111100,           // 'j', ord 0x6A
    TM1640_UNKNOWN_CHAR,  // 'k', ord 0x6B
    0b00001110,           // 'l', ord 0x6C
    TM1640_UNKNOWN_CHAR,  // 'm', ord 0x6D
    0b00010101,           // 'n', ord 0x6E
    0b00011101,           // 'o', ord 0x6F
    0b01100111,           // 'p', ord 0x70
    TM1640_UNKNOWN_CHAR,  // 'q', ord 0x71
    0b00000101,           // 'r', ord 0x72
    0b01011011,           // 's', ord 0x73
    0b00000111,           // 't', ord 0x74
    0b00011100,           // 'u', ord 0x75
    0b00011100,           // 'v', ord 0x76
    TM1640_UNKNOWN_CHAR,  // 'w', ord 0x77
    TM1640_UNKNOWN_CHAR,  // 'x', ord 0x78
    0b00100111,           // 'y', ord 0x79
    TM1640_UNKNOWN_CHAR,  // 'z', ord 0x7A
    0b00110001,           // '{', ord 0x7B
    0b00000110,           // '|', ord 0x7C
    0b00000111,           // '}', ord 0x7D
    0b01100011,           // '~', ord 0x7E (degree symbol)
};

void TM1640Display::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TM1640...");

  this->clk_pin_->setup();
  this->clk_pin_->digital_write(true);  // CLK high idle
  this->din_pin_->setup();
  this->din_pin_->digital_write(true);  // DIN high idle

  this->display();
}

void TM1640Display::dump_config() {
  ESP_LOGCONFIG(TAG,
                "TM1640:\n"
                "  Intensity: %d\n"
                "  Inverted: %d\n"
                "  Length: %d",
                this->intensity_, this->inverted_, this->length_);
  LOG_PIN("  CLK Pin: ", this->clk_pin_);
  LOG_PIN("  DIN Pin: ", this->din_pin_);
  LOG_UPDATE_INTERVAL(this);
}

void TM1640Display::update() {
  for (uint8_t &i : this->buffer_)
    i = 0;
  if (this->writer_.has_value())
    (*this->writer_)(*this);
  this->display();
}

float TM1640Display::get_setup_priority() const { return setup_priority::PROCESSOR; }

void TM1640Display::bit_delay_() { delayMicroseconds(10); }

void TM1640Display::start_() {
  // Start condition: when CLK is high, DIN goes from high to low
  this->din_pin_->digital_write(true);
  this->clk_pin_->digital_write(true);
  this->bit_delay_();
  this->din_pin_->digital_write(false);
  this->bit_delay_();
  this->clk_pin_->digital_write(false);
  this->bit_delay_();
}

void TM1640Display::stop_() {
  // Stop condition: when CLK is high, DIN goes from low to high
  this->clk_pin_->digital_write(false);
  this->din_pin_->digital_write(false);
  this->bit_delay_();
  this->clk_pin_->digital_write(true);
  this->bit_delay_();
  this->din_pin_->digital_write(true);
  this->bit_delay_();
}

void TM1640Display::display() {
  ESP_LOGVV(TAG, "Display %02X%02X%02X%02X%02X%02X%02X%02X", 
            buffer_[0], buffer_[1], buffer_[2], buffer_[3],
            buffer_[4], buffer_[5], buffer_[6], buffer_[7]);

  // Write DATA command (auto-increment mode)
  this->start_();
  this->send_byte_(TM1640_CMD_DATA);
  this->stop_();

  // Write ADDR command + data bytes
  this->start_();
  this->send_byte_(TM1640_CMD_ADDR);  // Start at address 0

  // Write the data bytes
  if (this->inverted_) {
    for (int8_t i = this->length_ - 1; i >= 0; i--) {
      this->send_byte_(this->buffer_[i]);
    }
  } else {
    for (uint8_t i = 0; i < this->length_; i++) {
      this->send_byte_(this->buffer_[i]);
    }
  }
  this->stop_();

  // Write display control command (on/off + brightness)
  this->start_();
  this->send_byte_(TM1640_CMD_CTRL | (this->intensity_ & 0x07) | (this->on_ ? 0x08 : 0x00));
  this->stop_();
}

void TM1640Display::send_byte_(uint8_t b) {
  uint8_t data = b;

  // Send 8 bits, LSB first
  for (uint8_t i = 0; i < 8; i++) {
    // CLK low
    this->clk_pin_->digital_write(false);
    this->bit_delay_();

    // Set data bit
    this->din_pin_->digital_write(data & 0x01);
    data >>= 1;
    this->bit_delay_();

    // CLK high
    this->clk_pin_->digital_write(true);
    this->bit_delay_();
  }

  // TM1640: After sending byte, CLK and DIN should be low
  this->clk_pin_->digital_write(false);
  this->din_pin_->digital_write(false);
  this->bit_delay_();
}

uint8_t TM1640Display::print(uint8_t start_pos, const char *str) {
  uint8_t pos = start_pos;
  bool use_dot = false;

  for (; *str != '\0'; str++) {
    uint8_t data = TM1640_UNKNOWN_CHAR;
    if (*str >= ' ' && *str <= '~')
      data = progmem_read_byte(&TM1640_ASCII_TO_RAW[*str - ' ']);

    if (data == TM1640_UNKNOWN_CHAR) {
      ESP_LOGW(TAG, "Encountered character '%c' with no TM1640 representation while translating string!", *str);
    }

    // Remap segments for compatibility with XABCDEFG format to TM1640 XGFEDCBA format
    if (this->inverted_) {
      // XABCDEFG > XGCBAFED
      data = ((data & 0x80) || use_dot ? 0x80 : 0) |  // X (dot)
             ((data & 0x40) ? 0x08 : 0) |             // A
             ((data & 0x20) ? 0x10 : 0) |             // B
             ((data & 0x10) ? 0x20 : 0) |             // C
             ((data & 0x08) ? 0x01 : 0) |             // D
             ((data & 0x04) ? 0x02 : 0) |             // E
             ((data & 0x02) ? 0x04 : 0) |             // F
             ((data & 0x01) ? 0x40 : 0);              // G
    } else {
      // XABCDEFG > XGFEDCBA
      data = ((data & 0x80) ? 0x80 : 0) |  // X (dot)
             ((data & 0x40) ? 0x01 : 0) |  // A
             ((data & 0x20) ? 0x02 : 0) |  // B
             ((data & 0x10) ? 0x04 : 0) |  // C
             ((data & 0x08) ? 0x08 : 0) |  // D
             ((data & 0x04) ? 0x10 : 0) |  // E
             ((data & 0x02) ? 0x20 : 0) |  // F
             ((data & 0x01) ? 0x40 : 0);   // G
    }

    use_dot = *str == '.';
    if (use_dot) {
      if ((!this->inverted_) && (pos != start_pos)) {
        this->buffer_[pos - 1] |= 0b10000000;
      }
    } else {
      if (pos >= 16) {
        ESP_LOGE(TAG, "String is too long for the display!");
        break;
      }
      this->buffer_[pos++] = data;
    }
  }
  return pos - start_pos;
}

uint8_t TM1640Display::print(const char *str) { return this->print(0, str); }

uint8_t TM1640Display::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}

uint8_t TM1640Display::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}

uint8_t TM1640Display::strftime(uint8_t pos, const char *format, ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}

uint8_t TM1640Display::strftime(const char *format, ESPTime time) { return this->strftime(0, format, time); }

}  // namespace tm1640
}  // namespace esphome
