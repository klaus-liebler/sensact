// SPDX-License-Identifier: MIT
//
// NXP PCA9685 16-channel PWM controller (datasheet rev. 4, 2015) as an I2C device for Renode,
// compiled at run time by sensactup02.resc:
//   pca0: Miscellaneous.PCA9685 @ i2c2 0x40
//
// Modelled: the register file with its reset values (MODE1 0x11: SLEEP + ALLCALL, MODE2 0x04:
// totem pole, every LEDn_OFF_H 0x10: full off, PRE_SCALE 0x1E); a write sets the register pointer
// (first byte) and writes the rest, auto-incrementing with MODE1.AI (from LED0_ON_L to the last LED
// register it wraps; without AI every byte goes to the same register); PRE_SCALE only takes a
// write while SLEEP is set; a read returns the registers from the pointer. RESTART and the
// oscillator are not modelled. Duty(n) gives the duty of channel n in 1/4096 as programmed (full
// on/off included, before MODE2.INVRT), Dump() all of it for the monitor:
//   sysbus.i2c2.pca0 Dump
using System;
using System.Text;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.I2C;

namespace Antmicro.Renode.Peripherals.Miscellaneous
{
    public class PCA9685 : II2CPeripheral
    {
        public PCA9685()
        {
            Reset();
        }

        public void Reset()
        {
            Array.Clear(regs, 0, regs.Length);
            regs[Mode1] = 0x11;
            regs[Mode2] = 0x04;
            for(var ch = 0; ch < 16; ch++)
            {
                regs[Led0 + 4 * ch + 3] = 0x10;
            }
            regs[0xFD] = 0x10;  // ALL_LED_OFF_H
            regs[PreScale] = 0x1E;
            pointer = 0;
            Writes = 0;
        }

        /// Register writes since reset.
        public ulong Writes { get; private set; }

        public void Write(byte[] data)
        {
            if(data.Length == 0)
            {
                return;
            }
            pointer = data[0];
            for(var i = 1; i < data.Length; i++)
            {
                WriteRegister(pointer, data[i]);
                Writes++;
                if((regs[Mode1] & Ai) != 0)
                {
                    pointer = NextPointer(pointer);
                }
            }
        }

        public byte[] Read(int count = 1)
        {
            var r = new byte[Math.Max(count, 0)];
            for(var i = 0; i < r.Length; i++)
            {
                r[i] = regs[pointer];
                if((regs[Mode1] & Ai) != 0)
                {
                    pointer = NextPointer(pointer);
                }
            }
            return r;
        }

        public void FinishTransmission()
        {
        }

        public byte Register(int address) => regs[address & 0xFF];

        /// Duty of channel `ch` in 1/4096 as programmed (before MODE2.INVRT inverts the pin).
        public int Duty(int ch)
        {
            var on = regs[Led0 + 4 * ch] | (regs[Led0 + 4 * ch + 1] << 8);
            var off = regs[Led0 + 4 * ch + 2] | (regs[Led0 + 4 * ch + 3] << 8);
            int duty;
            if((off & 0x1000) != 0)
            {
                duty = 0;
            }
            else if((on & 0x1000) != 0)
            {
                duty = 4096;
            }
            else
            {
                duty = ((off & 0xFFF) - (on & 0xFFF) + 4096) % 4096;
            }
            return duty;
        }

        public string Dump()
        {
            var sb = new StringBuilder();
            sb.AppendFormat("MODE1=0x{0:X2} MODE2=0x{1:X2}{2} PRE_SCALE={3} writes={4}\n", regs[Mode1], regs[Mode2],
                            (regs[Mode2] & Invrt) != 0 ? " (INVRT: pin inverted)" : "", regs[PreScale], Writes);
            for(var ch = 0; ch < 16; ch++)
            {
                sb.AppendFormat("LED{0}={1}/4096 ", ch, Duty(ch));
            }
            return sb.ToString();
        }

        private void WriteRegister(byte address, byte value)
        {
            if(address == PreScale && (regs[Mode1] & Sleep) == 0)
            {
                this.Log(LogLevel.Warning, "PRE_SCALE written while awake: ignored");
                return;
            }
            if(address == 0xFE || address == 0xFF || address >= 0x46 && address <= 0xF9)
            {
                if(address != PreScale)
                {
                    return;  // reserved / TestMode
                }
            }
            regs[address] = value;
        }

        private static byte NextPointer(byte p)
        {
            // auto-increment through the LED registers wraps back to LED0_ON_L (datasheet 7.3.1)
            if(p == 0x45)
            {
                return Led0;
            }
            return (byte)(p + 1);
        }

        private const byte Mode1 = 0x00, Mode2 = 0x01, Led0 = 0x06, PreScale = 0xFE;
        private const byte Ai = 0x20, Sleep = 0x10, Invrt = 0x10;

        private readonly byte[] regs = new byte[256];
        private byte pointer;
    }
}
