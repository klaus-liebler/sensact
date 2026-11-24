# sensact_firmware_f103 – Sensact-Station auf dem STM32F103 (rtoos)

Neue Firmware für die Unterputz-Knoten **sensactUp Rev 0.2** (STM32F103CBT6, 128 KB Flash, 20 KB RAM,
nur CAN): `SNSCT_L1_LVNG_UP` (Wohnzimmer) und `SNSCT_L1_KTCH_UP` (Küche). Sie ersetzt die
Altfirmware von 2019 (`../firmwareCubeIDE/modules`, `SENSACTUP02`).

- **Betriebssystem und Treiber:** [rtoos](https://github.com/klaus-liebler/rtoos) (C++26, GCC 16) mit
  dem Chip-Port `port/stm32f103` (Taktbaum, GPIO/AFIO, USART, I2C, bxCAN, Timer, IWDG). Die
  Konfiguration jedes Treibers ist ein Template-Argument und wird zur Compile-Zeit gegen RM0008 und
  das Datenblatt geprüft. Die Silicon-Errata aus ES096 sind eingearbeitet.
- **Apps:** dieselben wie auf den ESP32-Stationen (`espidf-components/sensact-applicationmodel`), also
  eine Implementierung mit einer Testsuite. Weggelassen werden nur Sound, Milight, Fingerprint und
  Jalousie-Timer (`SENSACT_WITHOUT_...`).
- **Konfiguration:** die Stationskonfiguration (BestBinaryBuffers) aus dem XML-Hausmodell,
  einkompiliert. Kein Upload, weil die Station nur am CAN hängt.

## Bauen

Voraussetzungen: GCC 16 (MSYS2 ucrt64), CMake, Ninja, .NET (Builder); `rtoos` und
`espidf-components` als Nachbarn von `sensact` ausgecheckt (`C:\repos\rtoos`,
`C:\repos\espidf-components`).

```
cd firmware/sensact_firmware
dotnet run --project builder -- GenerateStm32F103Files --model Sattlerstrasse16 --nodeId SNSCT_L1_LVNG_UP
cd ../sensact_firmware_f103
cmake --preset lvng && cmake --build --preset lvng          # -> build/lvng/sensact_f103.elf / .bin
```

Für die Küche: `--nodeId SNSCT_L1_KTCH_UP` und Preset `ktch`. Nach jeder Änderung am Hausmodell
den Builder-Schritt erneut ausführen; CMake baut dann neu.

Stand 2026-11-24 (LVNG): Flash 53,7 KB von 128 KB, davon 8 KB Stationskonfiguration. RAM 14,9 KB
von 20 KB: Stacks 3 + 1 KB, Init-Heap 8 KB, davon 5,3 KB belegt.

## Flashen

- **SWD** (Stecker P1: SWDIO, GND, SWCLK; ohne NRST, also „connect under reset“ mit Taster SW2) mit
  ST-Link und STM32CubeProgrammer: `STM32_Programmer_CLI -c port=SWD mode=UR -w build/lvng/sensact_f103.bin 0x08000000 -v -rst`.
  SWD bleibt jetzt eingeschaltet. Die Altfirmware schaltete es ab (`SWJ_DISABLE`), deshalb ist beim
  ersten Mal der Reset-Modus nötig.
- **Seriell** über den Bootloader am Stecker P4 (TX PA9, RX PA10, BOOT0, NRST, 3,3 V): BOOT0 high,
  Reset, dann z. B. `STM32_Programmer_CLI -c port=COMx -w ...bin 0x08000000`.

Konsole: USART1 an P4, 115200 Baud. Sie zeigt Start, Stationsname, angelegte Apps, Kommandos
(lokal/CAN) und jede Minute die Fehlerzähler von CAN, I2C und PCA9685.

## Platine sensactUp Rev 0.2

Aus der Leiterplatte (`pcb/sensactUp` im Initial-Commit `4491598`) abgelesen und gegen die
Altfirmware geprüft. Im Code steht das in [src/board/sensactup02.hpp](src/board/sensactup02.hpp); die
Pin-Datenbank von rtoos lehnt dort jeden Pin ab, der das Signal nicht hat.

| Pin | Funktion | Hausmodell (Bus 0 `INTERNAL`, Index = 16 x Port + Pin) |
|---|---|---|
| PB0, PB1, PA6, PA7, PA4, PA5, PA2, PA3 | Eingänge IO1–IO8 (Stecker O1 Pin 9–16), aktiv low | `IO1`…`IO8` = 16, 17, 6, 7, 4, 5, 2, 3 |
| PB15 / PB3 | Taster Drehgeber E1 / Stecker P19 | `ROT1_PUSH` 31, `ROT2_PUSH` 19 |
| PB14 | Optokoppler-Eingang U4 (P17) | `OPTO1` 30 |
| PA0/PA1 (TIM2), PB4/PB5 (TIM3 Partial Remap) | Drehgeber E1 / P19 | `ROTENC0`, `ROTENC1` |
| PB8, PB7, PB6, PB9 (TIM4 PWM, 1,1 kHz) | TPL7407L → O1 Pin 1, 3, 5, 7 | `O1` 24, `O3` 23, `O5` 22, `O7` 25 |
| PC13, PC14, PC15 | TPL7407L → O1 Pin 2, 4, 6 (an/aus) | `O2` 45, `O4` 46, `O6` 47 |
| PB13 | Optokoppler-Ausgang U7 (P18), aktiv low | `OPTO2` 29 |
| PB10/PB11 | I2C2 (4k7 Pull-ups, Stecker P11, DS2482), **50 kHz** | Bus 1 `BB`: PCA9685 `PCA0` 0x40 (`BB.0`–`BB.15`), `PCA1` 0x41 (`BB.16`–) |
| PA11/PA12 | CAN → SN65HVD230, 125 kbit/s | |
| PA9/PA10 | USART1 (P4) | |
| PB12 | Status-LED, aktiv low | |
| PA8, PA15 | WS2812-Kette, 5-V-Versorgung (SHDN von U1) | noch ungenutzt; 5 V aus (push-pull) |
| PD0 | 12-MHz-Oszillator (HSE-Bypass) → 72 MHz | |

Status-LED: kurz an jede Sekunde = normal; 5 Hz = keine gültige Konfiguration; lang an = CAN gestört
(error passive/bus off) oder 12-MHz-Oszillator ausgefallen.

## Aufbau

- **App-Thread** (Priorität 2): CAN-Kommandos sofort (Interrupt weckt ihn), alle 20 ms der App-Takt
  ([app_host.cpp](src/app_host.cpp), wie `cApplicationHost` der ESP32-Basis), CAN-Sendepuffer, LED,
  Watchdog.
- **IO-Thread** (Priorität 4): die PCA9685 ([pca9685_bank.hpp](src/pca9685_bank.hpp)).
- **I2C mit langen Kabeln:** Die Apps setzen nur Sollwerte; der IO-Thread überträgt geänderte Kanäle,
  liest jede Sekunde MODE1/MODE2 (ein durch Störung zurückgesetzter Chip wird neu eingerichtet) und
  schreibt alle 10 s alles neu. Jeder fehlgeschlagene Transfer stellt den Bus wieder her: bis zu
  neun SCL-Takte, die Sequenz aus ES096 2.8.7 gegen den hängenden BUSY-Zustand, SWRST. Ein
  fehlender Chip wird alle 200 ms erneut versucht. Ein gestörter Bus hält nie den App-Takt auf.
  Die Altfirmware wiederholte im App-Takt bis zu zehnmal und setzte nie SWRST.
- **Watchdog** (IWDG, 2–4 s): bedient nur, solange beide Threads laufen.
- **Speicher:** rtoos hat keinen Heap. Die Apps legen ihre Objekte beim Start mit `new` an; dafür gibt
  es einen festen Bereich ([heap.cpp](src/heap.cpp)), der danach versiegelt wird. Ein `new` im
  Betrieb hält den Knoten an, der Watchdog startet ihn neu.
- **CAN:** altes CAN-ID-Schema mit 29-bit-IDs wie die ESP32-Stationen (`USE_NEW_CAN_ID = false`):
  Kommandos an fremde Apps gehen auf den Bus, Kommandos an lokale Apps direkt an die App. In
  diesem Schema gibt es keine Status- und Event-Nachrichten.
- **Nicht unterstützt:** RGB-LEDs (WS2812), Ton, Uhrzeit (zeitgesteuerte Apps laufen auf ESP32-Stationen),
  gespeicherte App-Parameter, PCA9555-Eingänge am Bus.

## Emulator

Renode mit der Platine ([emulation/sensactup02.repl](emulation/sensactup02.repl): rtoos-Plattform des
F103, 72 MHz, bxCAN, zwei PCA9685 nach eigenem Modell [PCA9685.cs](emulation/PCA9685.cs)):

```
python -I emulation/run.py build/lvng/sensact_f103.elf --seconds 15 \
   --setup "sysbus.gpioPortA OnGPIO 2 true" \
   --at "7:sysbus.gpioPortA OnGPIO 2 false" --at "7.15:sysbus.gpioPortA OnGPIO 2 true" \
   --at "11:sysbus.i2c2.pca0 Dump"
```

Damit erprobt: Laden der Stationskonfiguration (17 Apps), ein Tastendruck IO7 schaltet den Dimmer
W1 ein, und PCA0 Kanal 12 steht danach auf voll an (MODE1 0x20, MODE2 0x10, PRE_SCALE 14). Kommandos an
fremde Apps (Rollladen J1, Steckdose C1) und Heartbeats gehen auf den CAN-Bus. Renode kennt keine
Pull-ups: Eingänge vor dem Start auf high setzen (`--setup`), sonst gelten alle Taster als gedrückt.

## Offen

- **Am Gerät erproben** (LVNG zuerst): Taster, Drehgeber (Richtung und Rastung: Zähler/2 wie in der
  Altfirmware), Lampen über den PCA9685, CAN zu den ESP32-Stationen, Verhalten bei abgezogenem und
  gestörtem I2C-Kabel (Konsole: `recover`, `reinits`).
- **Siliziumrevision des Chips:** Revision Z/B (ab 2008) verträgt laut ES096 2.2.9 Code neuerer
  Compiler nicht. Bei einer Platine von 2015 ist das unwahrscheinlich; die Kennung auf dem Gehäuse
  prüfen (Y, X, 1, 2, 3 sind in Ordnung).
- WS2812-Statusanzeige (TIM1_CH1 mit DMA), PCA9555-Eingänge, Status-/Event-Nachrichten mit dem
  neuen CAN-ID-Schema.
