#!/usr/bin/env python3
"""Firmware der sensactUp-Stationen (STM32F103) ueber den CAN-Bus flashen.

Das Paket ist das .bin aus dem Build (build/<preset>/sensact_f103.bin): Basis-Firmware und
einkompilierte Stationskonfiguration (Hausmodell) in einem, mit Image-Kopf (Station, Laenge, CRC32).
Protokoll und Speicheraufteilung: ../boot/boot_protocol.hpp.

  # nach dem Bauen (CMake ruft das selbst auf): Laenge, CRC und Bauzeit in den Kopf
  python -I tools/sensact_f103_flash.py seal build/lvng/sensact_f103.bin [--elf ... --objcopy ...]
  python -I tools/sensact_f103_flash.py info build/lvng/sensact_f103.bin

  # flashen: die Station per START_IAP in den Bootloader schicken (10 s Fenster) und uebertragen
  python -I tools/sensact_f103_flash.py flash build/lvng/sensact_f103.bin --interface gs_usb --channel 0
  # Station antwortet nicht (Firmware kaputt): Werkzeug starten, dann die Station aus- und
  # einschalten - der Bootloader wartet nach jedem Reset 1 s
  python -I tools/sensact_f103_flash.py flash ... --wait 120
  # Station ohne gueltige Firmware (meldet sich als 255)
  python -I tools/sensact_f103_flash.py flash ... --station 255

  python -I tools/sensact_f103_flash.py query --station 5 ...        # Bootloader abfragen (nach reboot --bootloader)
  python -I tools/sensact_f103_flash.py reboot --station 5 [--bootloader] ...

CAN-Adapter ueber python-can (pip install python-can): --interface gs_usb (candleLight), slcan
(--channel COM5), pcan (--channel PCAN_USBBUS1), socketcan (--channel can0), ... 125 kbit/s.
"""
from __future__ import annotations

import argparse
import os
import struct
import sys
import time
import zlib
from dataclasses import dataclass
from pathlib import Path

# ---- wie boot/boot_protocol.hpp ----------------------------------------------------------------
PROTOCOL_VERSION = 1
APP_BASE = 0x0800_4000
APP_MAX_SIZE = 112 * 1024
HEADER_OFFSET = 0x100
HEADER_FORMAT = "<IHHHHIII32s"  # magic, version, board, node, reserved, size, crc, build_time, name
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)
CRC_OFFSET = HEADER_OFFSET + 16
APP_MAGIC = 0x3150_4153
HEADER_VERSION = 1
BOARD_SENSACTUP02 = 1
NODE_UNCONFIGURED = 0xFF
CAN_COMMAND, CAN_REPLY, CAN_TYPE_MASK = 0x0200_0000, 0x0300_0000, 0x1F00_0000
WINDOW_FRAMES, DATA_PER_FRAME = 16, 7
HELLO, QUERY, BEGIN, DATA, END, START = 0x10, 0x11, 0x12, 0x13, 0x14, 0x15
STATUS = ["OK", "BAD_STATE", "BAD_SIZE", "WRONG_NODE", "FLASH_ERROR", "BAD_LENGTH", "BAD_CRC", "BAD_IMAGE", "NO_APP"]
INFO_APP_VALID, INFO_STAY, INFO_RECEIVING = 1, 2, 4
# Kommandos an Apps (eCommandType, altes CAN-ID-Schema: ID = App-Nummer, erstes Byte = Kommando)
CMD_START_IAP, CMD_RESET = 1, 3

assert HEADER_SIZE == 56


@dataclass
class Header:
    magic: int
    version: int
    board: int
    node: int
    size: int
    crc: int
    build_time: int
    name: str

    @staticmethod
    def parse(image: bytes) -> "Header":
        if len(image) < HEADER_OFFSET + HEADER_SIZE:
            raise SystemExit("Datei zu kurz fuer einen Image-Kopf")
        magic, version, board, node, _res, size, crc, build_time, name = struct.unpack_from(HEADER_FORMAT, image, HEADER_OFFSET)
        return Header(magic, version, board, node, size, crc, build_time, name.split(b"\0", 1)[0].decode("ascii", "replace"))


def image_crc(image: bytes) -> int:
    return zlib.crc32(image[:CRC_OFFSET] + b"\0\0\0\0" + image[CRC_OFFSET + 4:]) & 0xFFFF_FFFF


def check(image: bytes) -> Header:
    h = Header.parse(image)
    if h.magic != APP_MAGIC or h.version != HEADER_VERSION:
        raise SystemExit("kein sensact-F103-Paket (Image-Kopf fehlt)")
    if h.board != BOARD_SENSACTUP02:
        raise SystemExit(f"Paket fuer Board {h.board}, erwartet sensactUp rev 0.2 ({BOARD_SENSACTUP02})")
    if h.size != len(image) or h.size > APP_MAX_SIZE:
        raise SystemExit(f"Laenge im Kopf {h.size}, Datei {len(image)} (hoechstens {APP_MAX_SIZE}) - Paket nicht versiegelt?")
    if image_crc(image) != h.crc:
        raise SystemExit("CRC des Pakets falsch - Paket beschaedigt oder nicht versiegelt")
    return h


# ---- seal / info ------------------------------------------------------------------------------

def cmd_seal(args: argparse.Namespace) -> int:
    path: Path = args.bin
    image = bytearray(path.read_bytes())
    if len(image) > APP_MAX_SIZE:
        raise SystemExit(f"{path}: {len(image)} Byte, der Firmware-Bereich hat {APP_MAX_SIZE}")
    h = Header.parse(bytes(image))
    if h.magic != APP_MAGIC:
        raise SystemExit(f"{path}: kein Image-Kopf bei +0x{HEADER_OFFSET:X} (Firmware ohne .app_header gelinkt?)")
    build_time = int(os.environ.get("SOURCE_DATE_EPOCH", time.time()))
    struct.pack_into("<I", image, HEADER_OFFSET + 12, len(image))
    struct.pack_into("<I", image, HEADER_OFFSET + 20, build_time)
    struct.pack_into("<I", image, CRC_OFFSET, 0)
    struct.pack_into("<I", image, CRC_OFFSET, image_crc(bytes(image)))
    path.write_bytes(image)
    h = check(bytes(image))
    if args.elf:
        # derselbe Kopf ins ELF (Debugger, Emulator): nur der Abschnitt .app_header aendert sich
        import subprocess
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            section = Path(tmp) / "app_header.bin"
            section.write_bytes(bytes(image[HEADER_OFFSET:HEADER_OFFSET + HEADER_SIZE]))
            subprocess.run([args.objcopy, f"--update-section=.app_header={section}", str(args.elf)], check=True)
    print(f"{path.name}: {h.name} (Station {h.node}), {h.size} Byte, CRC32 {h.crc:08X}")
    return 0


def cmd_info(args: argparse.Namespace) -> int:
    image = args.bin.read_bytes()
    h = check(image)
    built = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(h.build_time))
    print(f"{h.name}: Station {h.node}, {h.size} Byte, CRC32 {h.crc:08X}, gebaut {built}")
    return 0


# ---- CAN ----------------------------------------------------------------------------------------

def command_id(node: int, cmd: int) -> int:
    return CAN_COMMAND | (node << 16) | (cmd << 8)


def reply_id(node: int, cmd: int) -> int:
    return CAN_REPLY | (node << 16) | (cmd << 8)


class Link:
    """Senden und auf eine bestimmte Antwort des Bootloaders warten (alles andere am Bus ignorieren)."""

    def __init__(self, bus, verbose: bool = False):
        self.bus = bus
        self.verbose = verbose

    def send(self, arbitration_id: int, data: bytes) -> None:
        import can
        self.bus.send(can.Message(arbitration_id=arbitration_id, data=data, is_extended_id=True))

    def wait(self, accept, timeout: float):
        """Erste empfangene Nachricht, fuer die accept(id) wahr ist, oder None."""
        end = time.monotonic() + timeout
        while True:
            left = end - time.monotonic()
            if left <= 0:
                return None
            m = self.bus.recv(left)
            if m is None:
                return None
            if m.is_extended_id and accept(m.arbitration_id):
                return m

    def request(self, node: int, cmd: int, data: bytes, timeout: float):
        self.send(command_id(node, cmd), data)
        return self.wait(lambda i: i == reply_id(node, cmd), timeout)


def status_text(m) -> str:
    s = m.data[0] if len(m.data) else 255
    return STATUS[s] if s < len(STATUS) else f"Status {s}"


def describe_info(node: int, data: bytes) -> str:
    flags = data[2]
    crc = struct.unpack_from("<I", data, 4)[0]
    state = "gueltige Firmware" if flags & INFO_APP_VALID else "keine gueltige Firmware"
    return f"Bootloader {data[1]} (Protokoll {data[0]}) Station {node}: {state}, CRC32 {crc:08X}"


def open_bus(args: argparse.Namespace):
    try:
        import can
    except ImportError:
        raise SystemExit("python-can fehlt: pip install python-can")
    kwargs = {"interface": args.interface, "bitrate": args.bitrate}
    if args.channel is not None:
        kwargs["channel"] = args.channel
    return can.Bus(**kwargs)


def catch_bootloader(link: Link, node: int, args: argparse.Namespace):
    """Die Station in den Bootloader holen und dort festhalten. Ergebnis: (Station, Info-Daten)."""
    if not args.no_reboot and node != NODE_UNCONFIGURED:
        # START_IAP an die Knoten-App (altes CAN-ID-Schema): Neustart mit 10 s Fenster
        link.send(node, bytes([CMD_START_IAP]))
    print(f"warte auf den Bootloader der Station {node} (bis {args.wait:.0f} s; sonst Station aus- und einschalten)")
    m = link.wait(lambda i: i == reply_id(node, HELLO), args.wait)
    if m is None:
        # vielleicht wartet er schon (frueheres START_IAP): direkt fragen
        m = link.request(node, QUERY, b"", 0.5)
        if m is None:
            raise SystemExit(f"Station {node}: kein Bootloader gemeldet")
    # sofort festhalten - ohne START_IAP wartet er nur 1 s
    q = link.request(node, QUERY, b"", 0.5) or m
    if len(q.data) < 8 or q.data[0] != PROTOCOL_VERSION:
        raise SystemExit(f"Station {node}: Bootloader mit Protokoll {q.data[0] if q.data else '?'}, dieses Werkzeug spricht {PROTOCOL_VERSION}")
    print(describe_info(node, bytes(q.data)))
    return bytes(q.data)


def transfer(link: Link, node: int, image: bytes, h: Header, ack_timeout: float = 0.5, retries: int = 10,
             progress=None) -> int:
    """BEGIN, DATA-Fenster, END. Ergebnis: Station nach END (ein unkonfigurierter Knoten heisst danach wie das Paket)."""
    size = len(image)
    begin = struct.pack("<I", size)[:3] + bytes([h.node]) + struct.pack("<I", h.crc)
    m = link.request(node, BEGIN, begin, 15.0)  # Loeschen dauert bis ~4 s
    if m is None:
        raise SystemExit("BEGIN: keine Antwort")
    if m.data[0] != 0:
        raise SystemExit(f"BEGIN abgelehnt: {status_text(m)}")
    total = (size + DATA_PER_FRAME - 1) // DATA_PER_FRAME
    acked = 0
    failures = 0
    while acked < total:
        end = min(total, acked + WINDOW_FRAMES)
        for i in range(acked, end):
            chunk = image[i * DATA_PER_FRAME:(i + 1) * DATA_PER_FRAME]
            link.send(command_id(node, DATA), bytes([i & 0xFF]) + chunk)
        m = link.wait(lambda i: i == reply_id(node, DATA), ack_timeout)
        if m is None:
            failures += 1
            if failures > retries:
                raise SystemExit(f"DATA: keine Antwort nach Frame {acked} ({retries} Wiederholungen)")
            continue
        if m.data[0] != 0:
            raise SystemExit(f"DATA abgelehnt bei Frame {acked}: {status_text(m)}")
        acked = m.data[1] | (m.data[2] << 8)
        failures = 0
        if progress:
            progress(min(size, acked * DATA_PER_FRAME), size)
    # nach END heisst ein bisher unkonfigurierter Knoten wie seine neue Firmware
    link.send(command_id(node, END), b"")
    m = link.wait(lambda i: i in (reply_id(node, END), reply_id(h.node, END)), 3.0)
    if m is None:
        raise SystemExit("END: keine Antwort")
    if m.data[0] != 0:
        crc = struct.unpack_from("<I", bytes(m.data), 1)[0] if len(m.data) >= 5 else 0
        raise SystemExit(f"END: {status_text(m)} (CRC im Flash {crc:08X}, Paket {h.crc:08X})")
    return (m.arbitration_id >> 16) & 0xFF


def start(link: Link, node: int) -> None:
    m = link.request(node, START, b"", 1.0)
    if m is None or m.data[0] != 0:
        raise SystemExit(f"START: {status_text(m) if m else 'keine Antwort'}")


def cmd_flash(args: argparse.Namespace) -> int:
    image = args.bin.read_bytes()
    h = check(image)
    node = args.station if args.station is not None else h.node
    print(f"Paket {h.name} (Station {h.node}), {h.size} Byte, CRC32 {h.crc:08X}")
    with open_bus(args) as bus:
        link = Link(bus)
        info = catch_bootloader(link, node, args)
        current_crc = struct.unpack_from("<I", info, 4)[0]
        if info[2] & INFO_APP_VALID and current_crc == h.crc and not args.force:
            print("die Station hat dieses Paket schon - starte die Firmware (--force uebertraegt trotzdem)")
            start(link, node)
            return 0
        t0 = time.monotonic()

        def progress(done: int, size: int) -> None:
            print(f"\r{done * 100 // size:3d} %  {done:6d} / {size} Byte", end="", flush=True)

        node = transfer(link, node, image, h, progress=progress)
        print(f"\nuebertragen und geprueft in {time.monotonic() - t0:.1f} s")
        start(link, node)
        print(f"Station {node} startet {h.name}")
    return 0


def cmd_query(args: argparse.Namespace) -> int:
    with open_bus(args) as bus:
        m = Link(bus).request(args.station, QUERY, b"", 1.0)
        if m is None:
            print(f"Station {args.station}: kein Bootloader (laeuft die Firmware? dann: reboot --bootloader)")
            return 1
        print(describe_info(args.station, bytes(m.data)))
    return 0


def cmd_reboot(args: argparse.Namespace) -> int:
    with open_bus(args) as bus:
        link = Link(bus)
        link.send(args.station, bytes([CMD_START_IAP if args.bootloader else CMD_RESET]))
        if args.bootloader:
            m = link.wait(lambda i: i == reply_id(args.station, HELLO), 5.0)
            print(f"Station {args.station}: " + ("Bootloader wartet 10 s" if m else "kein Bootloader gemeldet"))
            return 0 if m else 1
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)

    def bus_args(p):
        p.add_argument("--interface", default="gs_usb", help="python-can-Interface (gs_usb, slcan, pcan, socketcan, ...)")
        p.add_argument("--channel", help="Kanal des Adapters (z. B. COM5, can0, PCAN_USBBUS1)")
        p.add_argument("--bitrate", type=int, default=125_000)

    p = sub.add_parser("seal", help="Laenge, CRC und Bauzeit in den Image-Kopf schreiben (nach dem Bauen)")
    p.add_argument("bin", type=Path)
    p.add_argument("--elf", type=Path, help="denselben Kopf auch in dieses ELF schreiben")
    p.add_argument("--objcopy", default="arm-none-eabi-objcopy")
    p.set_defaults(func=cmd_seal)

    p = sub.add_parser("info", help="Image-Kopf eines Pakets anzeigen und pruefen")
    p.add_argument("bin", type=Path)
    p.set_defaults(func=cmd_info)

    p = sub.add_parser("flash", help="Paket ueber CAN auf die Station uebertragen")
    p.add_argument("bin", type=Path)
    p.add_argument("--station", type=int, help="Station am Bus (Standard: die des Pakets; 255 = ohne Firmware)")
    p.add_argument("--wait", type=float, default=15.0, help="Sekunden, die auf den Bootloader gewartet wird")
    p.add_argument("--no-reboot", action="store_true", help="kein START_IAP senden (Station wird von Hand neu gestartet)")
    p.add_argument("--force", action="store_true", help="auch uebertragen, wenn die Station dieses Paket schon hat")
    bus_args(p)
    p.set_defaults(func=cmd_flash)

    p = sub.add_parser("query", help="den Bootloader einer Station abfragen")
    p.add_argument("--station", type=int, required=True)
    bus_args(p)
    p.set_defaults(func=cmd_query)

    p = sub.add_parser("reboot", help="Station neu starten (RESET) oder in den Bootloader (START_IAP)")
    p.add_argument("--station", type=int, required=True)
    p.add_argument("--bootloader", action="store_true")
    bus_args(p)
    p.set_defaults(func=cmd_reboot)

    args = ap.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
