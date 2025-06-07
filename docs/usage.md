# Usage

## Capturing data

Run the wrapper directly to stream human‑readable output:

```bash
./adxl345usb -p /dev/ttyACM0
```

Use `-s FILE` to record CSV data and `-t SEC` to stop after a given time. When
the firmware outputs two sensors (`time,x0,y0,z0,x1,y1,z1` header) the `--sensor`
option selects which set of axes to write (0 or 1, default 0). Add `--dual` to
store all six axes in the file (header `time,x0,y0,z0,x1,y1,z1`).

On startup the wrapper prints `Press Q to stop`. If `-s` is provided the
resulting file begins with `time,x,y,z` unless `--dual` is used, in which case
the full dual header is written. Without `-s` each line on the console is
formatted like `time = 0.100, x = 0.123, y = 0.456, z = 0.789`. Use `--dual` to
show both sensors in this human readable mode. When saving to a file the banner
and a short summary remain on the console.

## Using the simulator

The repository ships with a simulator for development without hardware. Start it
and point the wrapper to the shown pseudo‑terminal. Pass `--dual` to the
simulator to mimic firmware built with `DUAL_SPI`:

```bash
./simulator.py        # single output
./simulator.py --dual # dual output
./adxl345usb -p /dev/pts/5 -t 1
```

The wrapper will behave exactly as if a real board was connected.

## Sample output

Running the wrapper against the simulator produces output like:

```text
Press Q to stop
time = 0.010, x = 0.786, y = -0.904, z = 0.174
time = 0.020, x = -0.611, y = 0.728, z = -0.441
time = 0.030, x = -0.483, y = 0.952, z = -0.585
...
Captured 87 samples in 1.01 s (86.3 Hz)
```
