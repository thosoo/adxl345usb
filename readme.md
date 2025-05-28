# ADXL345USB

## Overview

This project provides an installation script and firmware for using a Fly-ADXL345-USB (RP2040) sensor module with [OctoPrint-Pinput\_Shaping](https://github.com/navaismo/Octoprint-Pinput_Shaping) by navaismo.

## Features

* Supports ADXL345-USB via RP2040 (Fly-ADXL345-USB) for resonance testing.
* Compatible with OctoPrint’s Pinput Shaping plugin without any code modification.
* The installation script automatically sets up the required wrapper and configures permissions.

## Requirements

* OctoPrint installed (preferably in a virtual environment).
* A Fly-ADXL345-USB or any compatible RP2040 module with an ADXL345 sensor.
* Python (in OctoPrint’s venv) with PySerial.

## Installation

1. Ensure you have the ADXL345USB firmware flashed on your Fly-ADXL345-USB (RP2040).
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

The firmware running on the RP2040 should be the `adxl345usb` firmware provided in this project. It outputs CSV-formatted accelerometer data directly over USB (same format as `adxl345spi`).

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
