import os, subprocess, sys
WRAPPER = os.path.abspath("adxl345usb")

def run(*args):
    """Run the wrapper and return (code, stdout, stderr) as strings."""
    cp = subprocess.run([WRAPPER, *args],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True)
    return cp.returncode, cp.stdout, cp.stderr

def test_import_pyserial_is_missing():
    """
    The wrapper must exit 2 and print an informative message
    if 'pyserial' cannot be imported.
    We launch it in isolated mode so site-packages are hidden.
    """
    env = os.environ.copy()
    env["PYTHONPATH"] = ""          # ignore project root
    env["PYTHONNOUSERSITE"] = "1"   # ignore ~/.local

    cp = subprocess.run(
        [sys.executable, "-I", "-S", WRAPPER, "-p", "/dev/null"],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env)

    assert cp.returncode == 2
    assert "pyserial not installed" in cp.stderr.lower()

def test_bad_frequency():
    code, _, err = run("-f", "0")     # below 1 Hz
    assert code == 1
    assert "frequency" in err.lower()

def test_port_does_not_exist():
    code, _, err = run("-p", "/dev/ttyDoesNotExist")
    assert code == 2
    assert "cannot open" in err.lower()
