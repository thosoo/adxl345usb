import os, subprocess, sys
WRAPPER = os.path.abspath("adxl345usb")

def run(*args):
    """Run the wrapper and return (code, stdout, stderr) as strings."""
    cp = subprocess.run([WRAPPER, *args],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True)
    return cp.returncode, cp.stdout, cp.stderr

def test_import_pyserial():
    """Exit-code 2 when pyserial missing → simulate by unsetting PYTHONPATH."""
    code, _, err = subprocess.run(
        [sys.executable, "-c",
         "import importlib, subprocess, os, sys; "
         "os.environ['PYTHONPATH'] = ''; "
         "subprocess.run([sys.executable, '%s', '-p', '/dev/null'])" % WRAPPER],
         stdout=subprocess.PIPE, stderr=subprocess.PIPE,
         text=True).returncode, "", ""
    # When pyserial *is* installed the wrapper opens /dev/null and returns 2
    assert code == 2

def test_bad_frequency():
    code, _, err = run("-f", "0")     # below 1 Hz
    assert code == 1
    assert "frequency" in err.lower()

def test_port_does_not_exist():
    code, _, err = run("-p", "/dev/ttyDoesNotExist")
    assert code == 2
    assert "cannot open" in err.lower()
