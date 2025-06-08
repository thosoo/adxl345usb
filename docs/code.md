# Code Overview

This document outlines the main components of the project so newcomers can quickly understand how everything fits together.

## Wrapper (`adxl345usb`)

The wrapper is a small Python script that talks to the RP2040 board over USB. It
configures the sampling frequency, reads the CSV stream from the firmware and
either prints human readable lines or writes them to a CSV file. The wrapper is
also compatible with OctoPrint's Pinput Shaping plugin which expects the
`adxl345spi` style output.

It sends a text command `F=<Hz>` to the device to select the sample rate
(default `250` Hz) and then prints the banner `Press Q to stop`. When running
without `-s` the wrapper converts each CSV line from the firmware into a human
friendly message such as `time = 0.100, x = 0.123, y = 0.456, z = 0.789`.
With `-s` the header `time,x,y,z` and all raw values are written to the chosen
file while only the banner and a short summary appear on the console. When the
firmware outputs data for two sensors the wrapper selects one set of axes using
the `--sensor` option. To store or display all six axes, pass `--dual`.

Example wrapper output:

```text
Press Q to stop
time = 0.010, x = 0.786, y = -0.904, z = 0.174
time = 0.020, x = -0.611, y = 0.728, z = -0.441
```

## Firmware (`src/main.cpp`)

The firmware is written for the Arduino‑mbed RP2040 core. It sets up the ADXL345 over SPI and then streams acceleration samples as `time,x,y,z` lines. A simple text command interface allows changing the sample rate at runtime with `F=<Hz>`. When compiled with `DUAL_SPI` the output header becomes `time,x0,y0,z0,x1,y1,z1`.
Every time the frequency changes the firmware resends the appropriate header
followed by the data rows. Each row starts with the elapsed time in seconds and
then lists the accelerometer axes in units of `g`.

Example device output:

```text
time,x,y,z
0.010000,0.786000,-0.904000,0.174000
0.020000,-0.611000,0.728000,-0.441000
```

## Simulator (`simulator.py`)

For development without hardware, a simulator is provided. It creates a pseudo‑terminal that mimics the firmware's behaviour so the wrapper and plugin can be tested locally. Add `--dual` when running it to emit the dual‑sensor format. The tests use this simulator to validate the wrapper.

## Tests

The `tests` directory contains a minimal Pytest suite exercising both the wrapper and the simulator. Running `pytest` ensures that the wrapper behaves correctly even when PySerial is missing and that it can talk to the simulator.

Firmware sources are also tested using GoogleTest via PlatformIO. The `native` and `native_dual` environments build the firmware for the host, enabling single- and dual-sensor tests respectively. Run `pio test -e native -e native_dual` to execute them.
