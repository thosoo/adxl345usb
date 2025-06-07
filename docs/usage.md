# Usage

## Capturing data

Run the wrapper directly to stream human‑readable output:

```bash
./adxl345usb -p /dev/ttyACM0
```

Use `-s FILE` to record CSV data and `-t SEC` to stop after a given time.

## Using the simulator

The repository ships with a simulator for development without hardware. Start it
and point the wrapper to the shown pseudo‑terminal:

```bash
./simulator.py
./adxl345usb -p /dev/pts/5 -t 1
```

The wrapper will behave exactly as if a real board was connected.
