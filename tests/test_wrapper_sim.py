import subprocess, sys, time, re, os
from pathlib import Path
from sim_adxl import start as start_sim

WRAPPER = Path(__file__).parent.parent / "adxl345usb"

def run_wrapper(slave_path, *extra):
    """Launch the wrapper, capture stdout, run for 0.1 s, then send 'q'."""
    cp = subprocess.Popen(
        [WRAPPER, "-p", slave_path, "-f", "250", *extra],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        text=True
    )
    time.sleep(0.10)
    cp.stdin.write("q\n"); cp.stdin.flush()
    out, err = cp.communicate(timeout=2)
    return cp.returncode, out, err

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
    code, out, err = run_wrapper(pty_path, "-s", str(csv_file), "-t", "1")
    assert code == 0
    assert csv_file.exists() and csv_file.stat().st_size > 0
    # stdout should *only* contain the banner
    assert out.strip() == "Press Q to stop"
