// SPDX-License-Identifier: MIT
//
// Ein "anderer Knoten" am CAN-Hub fuer Renode-Tests: sendet Frames auf Befehl und zeichnet auf, was
// die anderen senden. Kompiliert von sensactup02.resc:
//   canInjector: CAN.CanInjector @ sysbus <0x50000000, +0x10>     (ein Platz am Bus, damit der Monitor es findet)
//   emulation CreateCANHub "canHub"; connector Connect sysbus.can1 canHub; connector Connect sysbus.canInjector canHub
// Monitor:
//   sysbus.canInjector Send 0x0000016B "0F"         (29-bit-ID, Nutzdaten hex)
//   sysbus.canInjector Received                     (alle empfangenen Frames, "ID: daten")
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;
using Antmicro.Renode.Core;
using Antmicro.Renode.Core.CAN;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.Bus;

namespace Antmicro.Renode.Peripherals.CAN
{
    public class CanInjector : ICAN, IDoubleWordPeripheral
    {
        public uint ReadDoubleWord(long offset) => 0;
        public void WriteDoubleWord(long offset, uint value) { }

        public event Action<CANMessageFrame> FrameSent;

        public void OnFrameReceived(CANMessageFrame message)
        {
            var line = string.Format("0x{0:X8}: {1}", message.Id, Hex(message.Data));
            lock(received)
            {
                received.Add(line);
            }
            this.Log(LogLevel.Info, "received {0}", line);
        }

        public void Reset()
        {
            lock(received)
            {
                received.Clear();
            }
        }

        public void Send(uint id, string hexData)
        {
            var data = new byte[hexData.Length / 2];
            for(var i = 0; i < data.Length; i++)
            {
                data[i] = byte.Parse(hexData.Substring(2 * i, 2), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
            }
            FrameSent?.Invoke(new CANMessageFrame(id, data, true));
        }

        public string Received()
        {
            lock(received)
            {
                return string.Join("\n", received);
            }
        }

        private static string Hex(byte[] data)
        {
            var sb = new StringBuilder();
            foreach(var b in data)
            {
                sb.AppendFormat("{0:X2}", b);
            }
            return sb.ToString();
        }

        private readonly List<string> received = new List<string>();
    }
}
