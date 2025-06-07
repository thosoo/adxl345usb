# Setup Guide

This guide walks through preparing a board and installing the wrapper on a Linux host.

## Flash the firmware

1. Install [PlatformIO](https://platformio.org/) or open the repository with VS Code.
2. Connect the RP2040 board in bootloader mode.
3. Build the desired environment from `platformio.ini` and flash the produced `uf2` file.

Pre‑built binaries are also available from the GitHub Actions page.

## Install the wrapper

Copy `adxl345usb` and `install.sh` to the same directory and run:

```bash
sudo bash install.sh
```

The script installs the wrapper as `/usr/local/bin/adxl345spi` and configures a sudoers rule so the OctoPrint user can run it without a password.
