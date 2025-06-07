#!/usr/bin/env python3
"""Simulated ADXL345USB device for testing the wrapper.

This script creates a pseudo-serial port that mimics the firmware behaviour.
It prints the path of the virtual serial device and then waits for commands
from the wrapper. It responds to ``F=<freq>`` by changing the sample rate and
streams CSV formatted accelerometer data.
"""

import argparse
import os
import pty
import random
import select
import sys
import time


parser = argparse.ArgumentParser(description="Run a simulated ADXL345USB device")
parser.add_argument("-f", "--freq", type=int, default=250,
                    help="initial sample frequency (Hz)")
parser.add_argument("--dual", action="store_true",
                    help="output data for two sensors like DUAL_SPI firmware")
args = parser.parse_args()

master_fd, slave_fd = pty.openpty()
port = os.ttyname(slave_fd)
print(f"Simulated serial port: {port}")
sys.stdout.flush()

freq = args.freq
if freq < 1:
    freq = 1
if freq > 3200:
    freq = 3200
period = 1.0 / freq

t0 = time.time()
next_sample = t0 + period
buffer = b""


try:
    while True:
        # Check for commands from the wrapper
        rlist, _, _ = select.select([master_fd], [], [], 0.01)
        if master_fd in rlist:
            data = os.read(master_fd, 1024)
            if not data:
                break
            buffer += data
            while b"\n" in buffer:
                line, buffer = buffer.split(b"\n", 1)
                line = line.decode().strip().upper()
                if line.startswith("F="):
                    print(f"set frequency to {line[2:]} Hz")
                    try:
                        val = int(line[2:])
                        if 1 <= val <= 3200:
                            freq = val
                            period = 1.0 / freq
                            t0 = time.time()
                            next_sample = t0 + period
                            if args.dual:
                                os.write(master_fd, b"time,x0,y0,z0,x1,y1,z1\n")
                            else:
                                os.write(master_fd, b"time,x,y,z\n")
                    except ValueError:
                        pass

        now = time.time()
        if now >= next_sample:
            t = now - t0
            if args.dual:
                x0 = random.uniform(-1.0, 1.0)
                y0 = random.uniform(-1.0, 1.0)
                z0 = random.uniform(-1.0, 1.0)
                x1 = random.uniform(-1.0, 1.0)
                y1 = random.uniform(-1.0, 1.0)
                z1 = random.uniform(-1.0, 1.0)
                line = (f"{t:.6f},{x0:.6f},{y0:.6f},{z0:.6f},"
                        f"{x1:.6f},{y1:.6f},{z1:.6f}\n").encode()
                os.write(master_fd, line)
            else:
                x = random.uniform(-1.0, 1.0)
                y = random.uniform(-1.0, 1.0)
                z = random.uniform(-1.0, 1.0)
                line = f"{t:.6f},{x:.6f},{y:.6f},{z:.6f}\n".encode()
                os.write(master_fd, line)
            next_sample += period
finally:
    os.close(master_fd)
    os.close(slave_fd)

