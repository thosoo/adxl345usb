# Usage

## Capturing data

Run the wrapper directly to stream human‑readable output:

```bash
./adxl345usb -p /dev/ttyACM0
```

Use `-s FILE` to record CSV data and `-t SEC` to stop after a given time.

On startup the wrapper prints `Press Q to stop` and then the header
`time,x,y,z`. Without `-s` each subsequent line contains values formatted like
`time = 0.100, x = 0.123, y = 0.456, z = 0.789`. With `-s` those CSV rows are
stored verbatim in the specified file while the console only shows the banner
and a final summary.

## Using the simulator

The repository ships with a simulator for development without hardware. Start it
and point the wrapper to the shown pseudo‑terminal:

```bash
./simulator.py
./adxl345usb -p /dev/pts/5 -t 1
```

The wrapper will behave exactly as if a real board was connected.
