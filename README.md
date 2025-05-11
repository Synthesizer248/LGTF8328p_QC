# QC Decoy Controller

Advanced Quick Charge negotiator with multi-protocol support and user interface.

![QC Decoy Demo](docs/demo-image.jpg)

## Features
- Supports QC2.0, QC3.0, and QC3.0+ protocols
- OLED display interface
- Rotary encoder control
- Voltage selection 3.6V-20V
- Safety timeouts and validation

## Hardware Requirements
- LGT8F328P microcontroller
- SSD1306 OLED display (128x64)
- EC11 rotary encoder
- QC-enabled USB charger
- External 3.3V power supply

## Wiring Guide
See [Wiring Guide](wiring-guide.md)

## Installation
1. Clone repository
2. Install PlatformIO
3. Connect hardware
4. Build and upload:

```bash
pio run -t upload
```

## Usage
- Rotate encoder to select voltage
- Press encoder to switch protocols
- Long press (2s) for emergency shutdown

## Contributing
Pull requests welcome! See [CONTRIBUTING.md](docs/CONTRIBUTING.md)