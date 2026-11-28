"""Das Sende-Werkzeug tools/sensact_f103_flash.py gegen den Automaten des Bootloaders (boot_sim, aus
boot/boot_protocol.hpp gebaut): Versiegeln, Uebertragung mit verlorenen Frames und Antworten, START.

  python -I test_sensact_f103_flash.py <boot_sim>     (ctest ruft das auf; braucht python-can)
"""
import argparse
import importlib.util
import os
import queue
import random
import struct
import subprocess
import sys
import tempfile
import threading
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("flashtool", HERE.parent / "tools" / "sensact_f103_flash.py")
tool = importlib.util.module_from_spec(spec)
sys.modules["flashtool"] = tool  # dataclasses sucht das Modul dort
spec.loader.exec_module(tool)

try:
    import can
except ImportError:
    print("python-can fehlt - Test uebersprungen")
    sys.exit(77)

BOOT_SIM = None


class SimBus:
    """Ein Bus mit genau einem Teilnehmer: boot_sim. drop(msg, to_station) -> True verliert den Frame."""

    def __init__(self, station: int, app_valid: bool, drop=None):
        self.proc = subprocess.Popen([BOOT_SIM, str(station), "1" if app_valid else "0"], stdin=subprocess.PIPE,
                                     stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1)
        self.drop = drop or (lambda m, to: False)
        self.rx = queue.Queue()
        threading.Thread(target=self._reader, daemon=True).start()

    def _reader(self):
        for line in self.proc.stdout:
            ident, _, data = line.strip().partition(" ")
            m = can.Message(arbitration_id=int(ident, 16), data=bytes.fromhex(data), is_extended_id=True)
            if not self.drop(m, False):
                self.rx.put(m)

    def send(self, m):
        if self.drop(m, True) or self.proc.poll() is not None:
            return
        self.proc.stdin.write(f"{m.arbitration_id:08X} {bytes(m.data).hex()}\n")
        self.proc.stdin.flush()

    def recv(self, timeout):
        try:
            return self.rx.get(timeout=timeout)
        except queue.Empty:
            return None

    def started(self) -> str:
        self.proc.wait(timeout=5)
        return self.proc.stderr.read()


def make_package(path: Path, node: int, size: int, seed: int = 3) -> bytes:
    rng = random.Random(seed)
    image = bytearray(rng.randrange(256) for _ in range(size))
    struct.pack_into("<II", image, 0, 0x2000_5000, tool.APP_BASE + 0x201)
    struct.pack_into(tool.HEADER_FORMAT, image, tool.HEADER_OFFSET, tool.APP_MAGIC, tool.HEADER_VERSION,
                     tool.BOARD_SENSACTUP02, node, 0, 0, 0, 0, b"SNSCT_L1_LVNG_UP")
    path.write_bytes(image)
    tool.cmd_seal(argparse.Namespace(bin=path, elf=None, objcopy=None))
    return path.read_bytes()


class FlashToolTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.pkg = Path(self.tmp.name) / "sensact_f103.bin"

    def tearDown(self):
        self.tmp.cleanup()

    def test_seal_writes_size_and_crc(self):
        image = make_package(self.pkg, 5, 3001)
        h = tool.check(image)
        self.assertEqual((h.node, h.size, h.name), (5, 3001, "SNSCT_L1_LVNG_UP"))
        broken = bytearray(image)
        broken[2000] ^= 0x40
        with self.assertRaises(SystemExit):
            tool.check(bytes(broken))

    def _flash(self, station: int, app_valid: bool, drop=None, size=9_999):
        image = make_package(self.pkg, 5, size)
        h = tool.check(image)
        bus = SimBus(station, app_valid, drop)
        link = tool.Link(bus)
        hello = link.wait(lambda i: i == tool.reply_id(station, tool.HELLO), 2.0)
        self.assertIsNotNone(hello)
        node = tool.transfer(link, station, image, h, ack_timeout=0.2)
        tool.start(link, node)
        return bus.started()

    def test_flashes_a_package(self):
        self.assertIn("STARTED node=5", self._flash(5, True))

    def test_lost_frames_and_replies(self):
        counter = {"to": 0, "from": 0}

        def drop(m, to_station):
            if (m.arbitration_id >> 8) & 0xFF != tool.DATA:
                return False
            key = "to" if to_station else "from"
            counter[key] += 1
            return counter[key] % (41 if to_station else 7) == 0

        self.assertIn("STARTED node=5", self._flash(5, False, drop))

    def test_unconfigured_station_takes_the_number_of_the_package(self):
        self.assertIn("STARTED node=5", self._flash(tool.NODE_UNCONFIGURED, False, size=2_000))

    def test_wrong_station_is_refused(self):
        image = make_package(self.pkg, 6, 2_000)
        bus = SimBus(5, True)
        link = tool.Link(bus)
        with self.assertRaises(SystemExit) as e:
            tool.transfer(link, 5, image, tool.check(image))
        self.assertIn("WRONG_NODE", str(e.exception))
        bus.proc.kill()


if __name__ == "__main__":
    BOOT_SIM = sys.argv.pop(1)
    unittest.main()
