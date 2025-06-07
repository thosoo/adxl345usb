# Code Overview

This document outlines the main components of the project so newcomers can quickly understand how everything fits together.

## Wrapper (`adxl345usb`)

The wrapper is a small Python script that talks to the RP2040 board over USB. It
configures the sampling frequency, reads the CSV stream from the firmware and
either prints human readable lines or writes them to a CSV file. The wrapper is
also compatible with OctoPrint's Pinput Shaping plugin which expects the
`adxl345spi` style output.

It sends a text command `F=<Hz>` to the device to select the sample rate
(default `250` Hz) and prints a banner `Press Q to stop` followed by the CSV
header `time,x,y,z`. Each row contains a timestamp in seconds and the three
acceleration axes. When running without `-s` the wrapper converts these values
into a human friendly format such as `time = 0.100, x = 0.123, y = 0.456, z =
0.789`. When `-s` is specified the raw CSV lines are written to the given file
while the banner and a short summary stay on the console.

## Firmware (`src/main.cpp`)

The firmware is written for the Arduino‑mbed RP2040 core. It sets up the ADXL345 over SPI and then streams acceleration samples as `time,x,y,z` lines. A simple text command interface allows changing the sample rate at runtime with `F=<Hz>`.
Every time the frequency changes the firmware resends the header `time,x,y,z`
followed by the data rows. Each row starts with the elapsed time in seconds and
then lists the X, Y and Z axes in units of `g`.

## Simulator (`simulator.py`)

For development without hardware, a simulator is provided. It creates a pseudo‑terminal that mimics the firmware's behaviour so the wrapper and plugin can be tested locally. The tests use this simulator to validate the wrapper.

## Tests

The `tests` directory contains a minimal Pytest suite exercising both the wrapper and the simulator. Running `pytest` ensures that the wrapper behaves correctly even when PySerial is missing and that it can talk to the simulator.
