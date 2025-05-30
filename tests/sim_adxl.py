"""
A minimal ADXL345 *simulator* – writes a header + fake CSV rows to the
master side of a pseudo-terminal so the wrapper believes it is talking to
real firmware.
"""

import os, pty, threading, time, math, random, sys

SPS = 250               # samples/second
AMP = 1e-2              # ±0.01 g peak

def _writer(fd):
    """background thread that streams CSV forever"""
    os.write(fd, b"time,x,y,z\n")          # firmware header
    t0 = time.time()
    while True:
        t  = time.time() - t0
        # simple sine on X, small noise on Y/Z
        x  = AMP * math.sin(2*math.pi*50*t)
        y  = random.uniform(-AMP/2, AMP/2)
        z  = 0.98 + random.uniform(-AMP/2, AMP/2)
        line = f"{t:.5f},{x:.6f},{y:.6f},{z:.6f}\n".encode()
        os.write(fd, line)
        time.sleep(1/SPS)

def start():
    """
    Returns path to the *slave* side of the PTY.
    Any process that opens that path sees live CSV lines.
    """
    master, slave = pty.openpty()
    slave_path = os.ttyname(slave)
    os.close(slave)                      # ← release the slave F-descriptor
    threading.Thread(target=_writer, args=(master,), daemon=True).start()
    return slave_path
