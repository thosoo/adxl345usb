# Board SPI Pinouts

The firmware supports multiple boards. Each board uses specific GPIOs for the ADXL345's SPI connection. The same pin values are defined in `platformio.ini` via `build_flags`.

| Board / Environment | CS | SCK | MOSI | MISO |
|--------------------|----|----|----|----|
| Fly ADXL345-USB (Mellow) | 9 | 10 | 11 | 12 |
| Fysetc Portable Input-Shaper (PIS) | 13 | 10 | 11 | 12 |
| BTT ADXL345 | 9 | 10 | 11 | 8 |
| KUSBA v2 | 1 | 2 | 3 | 0 |
| Generic RP2040 board (SPI0) | 17 | 18 | 19 | 16 |
| Generic RP2040 board (SPI1) | 13 | 10 | 11 | 12 |
| Generic RP2040 board (dual) | 17/13 | 18/10 | 19/11 | 16/12 |

Refer to the comments in `platformio.ini` for a description of each environment. Custom boards can override the `CS_PIN`, `SCK_PIN`, `MOSI_PIN` and `MISO_PIN` macros via `build_flags`.
