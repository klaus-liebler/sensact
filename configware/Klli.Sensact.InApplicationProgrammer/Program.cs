using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace Klli.Sensact.InApplicationProgrammer
{
    class Program
    {
        static void Main(string[] args)
        {
            SerialPort _serialPort = new SerialPort();
            _serialPort.BaudRate = 115200;
            _serialPort.ReadTimeout = 500;
            _serialPort.WriteTimeout = 500;
            _serialPort.
        }
    }
}
