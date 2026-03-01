# ESPHome TM1640 Component

Custom ESPHome component for the **TM1640** LED display driver chip.

## Features

- Support for up to 16 seven-segment digits
- Adjustable brightness (0-7)
- Inverted display mode
- Compatible with the standard ESPHome display component API
- printf, print, and strftime support

## Differences from TM1637

| Feature | TM1637 | TM1640 |
|---------|--------|--------|
| Max digits | 6 | 16 |
| Protocol | 2-wire with ACK | 2-wire without ACK |
| Key scanning | Yes | No |
| Pins | DIO, CLK | DIN, CLK |

## Installation

Add to your ESPHome configuration:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/AntorFr/esphome-tm1640
      ref: main
    refresh: 0s
```

## Configuration Example

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/AntorFr/esphome-tm1640
      ref: main
    refresh: 0s

display:
  - platform: tm1640
    clk_pin: GPIO14
    din_pin: GPIO12
    intensity: 7          # 0-7, brightness level
    length: 16            # Number of digits (1-16)
    inverted: false       # Flip display orientation
    update_interval: 1s
    lambda: |-
      it.print("Hello");
```

## Wiring

| TM1640 Pin | ESP Pin |
|------------|---------|
| VCC | 3.3V or 5V |
| GND | GND |
| DIN | GPIO (e.g., GPIO12) |
| CLK | GPIO (e.g., GPIO14) |

## API

### print(text)
Print text at position 0.

```yaml
lambda: |-
  it.print("1234");
```

### print(pos, text)
Print text at a specific position.

```yaml
lambda: |-
  it.print(2, "AB");
```

### printf(format, ...)
Printf-style formatting.

```yaml
lambda: |-
  it.printf("%d", id(my_sensor).state);
```

### strftime(format, time)
Display time using strftime format.

```yaml
lambda: |-
  it.strftime("%H.%M", id(my_time).now());
```

### set_intensity(level)
Change brightness (0-7).

### set_on(bool)
Turn display on/off.

## Raw Buffer Access

For advanced use cases (custom symbols, LED matrices, etc.), you can directly manipulate the 16-byte display buffer.

### get_buffer()
Get pointer to the raw 16-byte display buffer.

```yaml
lambda: |-
  uint8_t *buf = it.get_buffer();
  buf[0] = 0xFF;  // Direct access
```

### set_raw(pos, value)
Write a raw byte at a specific buffer position (0-15), overwriting existing value.

```yaml
lambda: |-
  it.set_raw(3, 0x12);  // Set byte 3 to 0x12
```

### set_raw_or(pos, value)
OR a raw byte at a specific buffer position, combining with existing value (useful for setting individual bits).

```yaml
lambda: |-
  it.set_raw_or(3, 0x02);  // Set bit 1 without affecting other bits
```

### set_raw_and(pos, value)
AND a raw byte at a specific buffer position (useful for clearing specific bits).

```yaml
lambda: |-
  it.set_raw_and(3, 0xFD);  // Clear bit 1, keep others
```

### clear_raw(pos)
Clear a specific buffer position (set to 0).

```yaml
lambda: |-
  it.clear_raw(3);  // Clear byte 3
```

## Example: Xiaomi Smart Kettle 2 Pro

The TM1640 in the Xiaomi kettle has a specific memory map:

| Byte | Function |
|------|----------|
| 0 | Digit 1 (tens) - 7-segment |
| 1 | Digit 2 (units) - 7-segment |
| 3 | Symbols: bit1=WiFi, bit4=°C |
| 4-6 | RGB LEDs 1-7 (Orange/Blue/Red) |
| 7-9 | RGB LEDs 8-12 (Orange/Blue/Red) |

```yaml
display:
  - platform: tm1640
    clk_pin: GPIO14
    din_pin: GPIO12
    length: 10
    lambda: |-
      // Display "85°C" with WiFi icon
      it.print("85");
      it.set_raw_or(3, 0x02 | 0x10);  // WiFi + °C icons
      
      // Light up orange LEDs 1-7
      it.set_raw(4, 0x7F);
```

## Supported Characters

The component supports ASCII characters from space (0x20) to tilde (0x7E).
Some characters like `K`, `M`, `W`, `X` cannot be displayed on 7-segment displays.

## License

MIT License

## Credits

Based on:
- ESPHome TM1637 component
- [rjbatista/tm1638-library](https://github.com/rjbatista/tm1638-library)
- [maxint-rd/TM16xx](https://github.com/maxint-rd/TM16xx)
