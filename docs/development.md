# Development

## Building the firmware

PlatformIO environments for the supported boards are defined in `platformio.ini`.
Run `pio run -e <env>` to build a firmware or `pio run -e <env> -t upload` to
flash it directly when your board is connected.

## Running the tests

Install the dependencies and run the Python tests with `pytest`:

```bash
pip install pyserial pytest
pytest -q
```

The Python test suite exercises the wrapper and the simulator.

### Firmware unit tests

The firmware can be tested on the host using PlatformIO and GoogleTest. Install
PlatformIO alongside the Python dependencies and run:

```bash
pip install platformio
pio test -e native -e native_dual
```
