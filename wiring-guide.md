# Wiring Guide

## Component Connections

| Component       | Microcontroller Pin | Notes                 |
|-----------------|---------------------|-----------------------|
| OLED SDA        | A4                  | I2C communication     |
| OLED SCL        | A5                  |                       |
| Encoder CLK     | D2                  | Interrupt-enabled pin |
| Encoder DT      | D3                  |                       |
| Encoder SW      | D4                  | Button input          |
| QC D+           | A0                  | DAC output            |
| QC D-           | A1                  | DAC output            |
| Power           | 3.3V                | External supply       |