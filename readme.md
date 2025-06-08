# ADXL345USB

## Overview

This project provides an installation script and firmware for using a RP2040 based ADXL345 sensor module with [OctoPrint-Pinput\_Shaping](https://github.com/navaismo/Octoprint-Pinput_Shaping) by navaismo.

See the [docs](docs/) directory for detailed guides on setup, usage, development and the code itself. SPI pinouts for the supported boards are listed in [docs/spi-ports.md](docs/spi-ports.md).

## Features

* Supports ADXL345 via RP2040 boards for resonance testing. Supported boards include Fly ADXL345-USB (Mellow), BTT ADXL345, FYSETC Portable Input-Shaper (PIS), KUSBA v2 and generic RP2040 boards (SPI0 or SPI1).
* Compatible with OctoPrint’s Pinput Shaping plugin without any code modification.
* The installation script automatically sets up the required wrapper and configures permissions.

## Requirements

* OctoPrint installed (preferably in a virtual environment).
* Python (in OctoPrint’s venv) with PySerial.

## Installation

1. Ensure you have the ADXL345USB firmware flashed on your sensor module.
2. Copy both the `adxl345usb` script and the `install.sh` script to a directory.
3. Run the installation script as `su`:

   ```bash
   su
   ./install.sh
   ```

## Usage

* The `adxl345usb` wrapper is installed as `adxl345spi` in `/usr/local/bin/`.
* The OctoPrint user has direct permission to use this tool without sudo.
* In OctoPrint-Pinput\_Shaping, point the tool path to `/usr/local/bin/adxl345spi`.

## Firmware Configuration

The firmware running on the RP2040 should be the `adxl345usb` firmware provided in this project. The firmware can be either compiled using VS Code and PlatformIO or downloaded from the GitHub action tab. It outputs CSV-formatted accelerometer data directly over USB (same format as `adxl345spi`). When compiled with `DUAL_SPI` it streams `time,x0,y0,z0,x1,y1,z1` lines which the wrapper can save using the `--dual` option.

## Simulator

A simple simulator (`simulator.py`) is included for experimenting with the wrapper without any hardware attached. Run the simulator and point the wrapper to the printed pseudo-serial device:

```bash
./simulator.py
  ./adxl345usb -p /dev/pts/5
```

## Testing

Python tests verify the wrapper and simulator, while PlatformIO builds run
GoogleTest-based firmware tests. Install the dependencies and run:

```bash
pip install pyserial pytest platformio
pytest -q
pio test -e native -e native_dual
```

## Troubleshooting

* If you receive a "Permission Denied" error, ensure that the OctoPrint user has execute permissions on the tool:

  ```bash
  chmod 750 /usr/local/bin/adxl345spi
  chown octoprint:octoprint /usr/local/bin/adxl345spi
  ```

* Make sure the `adxl345usb` script is in the same directory as the install script during installation.

## License

This project is open-source under the MIT License.

## Contributing

Feel free to open issues or submit pull requests.

Additional documentation (including a code overview) can be found in the [docs](docs/) directory.
