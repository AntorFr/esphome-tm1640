#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/time.h"
#include "esphome/components/display/display.h"

namespace esphome {
namespace tm1640 {

static const uint8_t TM1640_MAX_POS = 16;

class TM1640Display;

using tm1640_writer_t = display::DisplayWriter<TM1640Display>;

class TM1640Display : public PollingComponent {
 public:
  void set_writer(tm1640_writer_t &&writer) { this->writer_ = writer; }

  void setup() override;

  void dump_config() override;

  void set_clk_pin(GPIOPin *pin) { clk_pin_ = pin; }
  void set_din_pin(GPIOPin *pin) { din_pin_ = pin; }

  float get_setup_priority() const override;

  void update() override;

  /// Evaluate the printf-format and print the result at the given position.
  uint8_t printf(uint8_t pos, const char *format, ...) __attribute__((format(printf, 3, 4)));
  /// Evaluate the printf-format and print the result at position 0.
  uint8_t printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

  /// Print `str` at the given position.
  uint8_t print(uint8_t pos, const char *str);
  /// Print `str` at position 0.
  uint8_t print(const char *str);

  void set_intensity(uint8_t intensity) { this->intensity_ = intensity; }
  void set_inverted(bool inverted) { this->inverted_ = inverted; }
  void set_length(uint8_t length) { this->length_ = length; }
  void set_on(bool on) { this->on_ = on; }

  void display();

  /// Evaluate the strftime-format and print the result at the given position.
  uint8_t strftime(uint8_t pos, const char *format, ESPTime time) __attribute__((format(strftime, 3, 0)));
  /// Evaluate the strftime-format and print the result at position 0.
  uint8_t strftime(const char *format, ESPTime time) __attribute__((format(strftime, 2, 0)));

  /// Get pointer to the raw 16-byte display buffer
  uint8_t *get_buffer() { return this->buffer_; }

  /// Write a raw byte at a specific buffer position (0-15), overwriting existing value
  void set_raw(uint8_t pos, uint8_t value);

  /// OR a raw byte at a specific buffer position (0-15), combining with existing value
  void set_raw_or(uint8_t pos, uint8_t value);

  /// AND a raw byte at a specific buffer position (0-15), for clearing specific bits
  void set_raw_and(uint8_t pos, uint8_t value);

  /// Clear a specific buffer position
  void clear_raw(uint8_t pos);

 protected:
  void bit_delay_();
  void send_byte_(uint8_t b);
  void start_();
  void stop_();

  GPIOPin *din_pin_;
  GPIOPin *clk_pin_;
  uint8_t intensity_;
  uint8_t length_;
  bool inverted_;
  bool on_{true};
  tm1640_writer_t writer_{};
  uint8_t buffer_[16] = {0};
};

}  // namespace tm1640
}  // namespace esphome
