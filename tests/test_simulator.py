import os, pty, subprocess, sys
WRAPPER = os.path.abspath("adxl345usb")

def run_with_pty(*args):
    """Run the wrapper attached to a pseudo-terminal."""
    master, slave = pty.openpty()
    try:
        cp = subprocess.run([WRAPPER, *args], stdin=slave,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            text=True)
    finally:
        os.close(master)
        os.close(slave)
    return cp.returncode, cp.stdout, cp.stderr
def test_wrapper_with_simulator():
    sim = subprocess.Popen([sys.executable, "simulator.py"],
                           stdout=subprocess.PIPE,
                           text=True)
    try:
        port_line = sim.stdout.readline().strip()
        assert port_line.startswith("Simulated serial port:"), port_line
        port = port_line.split(":", 1)[1].strip()
        code, out, err = run_with_pty("-p", port, "-t", "1")
        assert code == 0
        assert "Press Q to stop" in out
        assert "Captured" in err
    finally:
        sim.terminate()
        try:
            sim.wait(timeout=5)
        except subprocess.TimeoutExpired:
            sim.kill()
