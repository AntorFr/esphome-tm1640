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
