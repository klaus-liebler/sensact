#!/usr/bin/env python3
"""Die Sensact-F103-Firmware in Renode laufen lassen und die Konsole (USART1) mitlesen.

  python -I emulation/run.py build/lvng/sensact_f103.elf --seconds 8
  python -I emulation/run.py build/lvng/sensact_f103.elf --expect "local applications" --expect "sealed"
  python -I emulation/run.py build/lvng/sensact_f103.elf --setup "sysbus.gpioPortA OnGPIO 2 true"          --at "3:sysbus.gpioPortA OnGPIO 2 false" --at "3.2:sysbus.gpioPortA OnGPIO 2 true"

--setup: Renode-Monitorbefehle vor dem Start (z. B. Eingaenge auf high: Renode kennt keine Pull-ups),
--at T:BEFEHL: ein Monitorbefehl T Sekunden (Wanduhr) nach dem Start - Taster druecken/loslassen.

Mit --expect endet der Lauf, sobald jede erwartete Zeile erschienen ist (Exit-Code 0), sonst nach
--seconds mit Exit-Code 1. Renode: C:/opt/renode (wie bei rtoos, s. dessen emulation/run.py).
"""
from __future__ import annotations

import argparse
import codecs
import re
import socket
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent


def free_port() -> int:
    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def symbol_address(elf: Path, symbol: str) -> int:
    nm = subprocess.run(["arm-none-eabi-nm", "-C", str(elf)], capture_output=True, text=True, check=True).stdout
    for line in nm.splitlines():
        parts = line.split(" ", 2)
        if len(parts) == 3 and parts[2] == symbol:
            return int(parts[0], 16)
    raise SystemExit(f"Symbol {symbol} nicht in {elf}")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("elf", type=Path)
    ap.add_argument("--seconds", type=float, default=10.0, help="Wanduhr-Zeit, nach der abgebrochen wird")
    ap.add_argument("--expect", action="append", default=[], help="Text, der in der Konsole erscheinen muss")
    ap.add_argument("--setup", action="append", default=[], help="Monitorbefehl vor dem Start")
    ap.add_argument("--at", action="append", default=[], help="SEKUNDEN:Monitorbefehl waehrend des Laufs")
    ap.add_argument("--renode", type=Path, default=Path("C:/opt/renode/renode.exe"))
    ap.add_argument("--renode-log", type=Path, help="Ausgabe von Renode (Log, Fehler) in diese Datei")
    args = ap.parse_args()
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")

    port, monitor_port = free_port(), free_port()
    commands = "; ".join([
        f"$bin=@{args.elf.resolve().as_posix()}",
        f"include @{(HERE / 'sensactup02.resc').as_posix()}",
        f'emulation CreateServerSocketTerminal {port} "console" false',
        "connector Connect sysbus.usart1 console",
        *args.setup,
        "start",
    ])
    timed = []
    for a in args.at:
        t, cmd = float(a.split(":", 1)[0]), a.split(":", 1)[1]
        if cmd.startswith("dump "):
            # "dump SYMBOL N": N 32-bit-Woerter der Variablen SYMBOL (C++-Name, arm-none-eabi-nm -C)
            _, symbol, count = cmd.split(" ", 2)
            address = symbol_address(args.elf, symbol)
            timed += [(t + 0.01 * i, f"sysbus ReadDoubleWord 0x{address + 4 * i:08X}") for i in range(int(count))]
        else:
            timed.append((t, cmd))
    timed.sort(key=lambda x: x[0])
    log = open(args.renode_log, "wb") if args.renode_log else subprocess.DEVNULL
    proc = subprocess.Popen([str(args.renode), "--disable-gui", "--plain", "--port", str(monitor_port), "-e", commands],
                            stdout=log, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL)
    try:
        deadline = time.monotonic() + 60
        while True:
            try:
                sock = socket.create_connection(("127.0.0.1", port), timeout=1)
                break
            except OSError:
                if proc.poll() is not None or time.monotonic() > deadline:
                    print("Renode hat die Konsole nicht geoeffnet", file=sys.stderr)
                    return 2
                time.sleep(0.2)
        sock.settimeout(0.2)
        monitor = socket.create_connection(("127.0.0.1", monitor_port), timeout=5) if timed else None
        start = time.monotonic()
        missing = list(args.expect)
        text = ""
        decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")
        monitor_text = ""
        end = time.monotonic() + args.seconds
        while time.monotonic() < end:
            while timed and time.monotonic() - start >= timed[0][0]:
                cmd = timed.pop(0)[1]
                monitor.sendall((cmd + "\n").encode())
                print(f"[monitor] {cmd}", flush=True)
            if monitor is not None:
                monitor.settimeout(0.05)
                try:
                    monitor_text += monitor.recv(65536).decode(errors="replace").replace("\r", "")
                    *lines, monitor_text = monitor_text.split("\n")
                    for line in lines:
                        # Antworten auf die eigenen Befehle; das Echo, Prompts und das Banner nicht
                        line = re.sub(r"\x1b\[[0-9;]*[A-Za-z]|[^\x20-\x7e]", "", line).strip()
                        if line and not line.startswith("(") and "sysbus." not in line and "Renode" not in line \
                                and "$bin" not in line:
                            print(f"[monitor] {line}", flush=True)
                except socket.timeout:
                    pass
            try:
                chunk = sock.recv(4096)
            except socket.timeout:
                continue
            if not chunk:
                break
            s = decoder.decode(chunk).replace("\r", "")
            sys.stdout.write(s)
            sys.stdout.flush()
            text += s
            missing = [e for e in missing if e not in text]
            if args.expect and not missing:
                return 0
        if missing:
            print(f"\n=> fehlt: {missing}", file=sys.stderr)
            return 1
        return 0
    finally:
        proc.kill()


if __name__ == "__main__":
    sys.exit(main())
