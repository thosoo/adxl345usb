# Development

## Building the firmware

PlatformIO environments for the supported boards are defined in `platformio.ini`.
Run `pio run -e <env>` to build a firmware or `pio run -e <env> -t upload` to
flash it directly when your board is connected.

## Running the tests

Install the dependencies and run `pytest`:

```bash
pip install pyserial pytest
pytest -q
```

The test suite exercises the wrapper and the simulator.
