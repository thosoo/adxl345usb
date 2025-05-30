import subprocess, sys, time, re, os
from pathlib import Path
from sim_adxl import start as start_sim

WRAPPER = Path(__file__).parent.parent / "adxl345usb"

def run_wrapper(slave_path, *extra):
    env = os.environ | {"ADXLUSB_BAUD": "115200"}
    cp = subprocess.run(
        [WRAPPER, "-p", slave_path, "-t", "0.2", *extra],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        text=True, timeout=2, env=env
    )
    return cp.returncode, cp.stdout, cp.stderr

def test_stream_and_quit():
    pty_path = start_sim()
    code, out, err = run_wrapper(pty_path)
    # should exit cleanly (0) and print at least one data row
    assert code == 0
    assert "Press Q to stop" in out.splitlines()[0]
    rows = [l for l in out.splitlines()[1:] if re.match(r'^time', l) is None]
    assert len(rows) >= 5          # received many samples

def test_csv_file_mode(tmp_path):
    pty_path = start_sim()
    csv_file = tmp_path / "cap.csv"
    code, out, err = run_wrapper(pty_path, "-s", str(csv_file))
    assert code == 0
    assert csv_file.exists() and csv_file.stat().st_size > 0
    # stdout should *only* contain the banner
    assert out.strip() == "Press Q to stop"
