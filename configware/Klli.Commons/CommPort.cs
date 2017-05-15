using System;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Windows.Forms;

namespace Klli.Sensact.TestGui
{
    /// <summary> CommPort class creates a singleton instance
    /// of SerialPort (System.IO.Ports) </summary>
    /// <remarks> When ready, you open the port.
    ///   <code>
    ///   CommPort com = CommPort.Instance;
    ///   com.StatusChanged += OnStatusChanged;
    ///   com.DataReceived += OnDataReceived;
    ///   com.Open();
    ///   </code>
    ///   Notice that delegates are used to handle status and data events.
    ///   When settings are changed, you close and reopen the port.
    ///   <code>
    ///   CommPort com = CommPort.Instance;
    ///   com.Close();
    ///   com.PortName = "COM4";
    ///   com.Open();
    ///   </code>
    /// </remarks>
    public sealed class CommPort
    {
        SerialPort _serialPort;
        Thread _readThread;
        volatile bool _keepReading;

        public CommPort()
        {
            _serialPort = new SerialPort();
            _serialPort.BaudRate = 115200;
            // Set the read/write timeouts
            _serialPort.ReadTimeout = 500;
            _serialPort.WriteTimeout = 500;

            _readThread = null;
            _keepReading = false;
        }

        public string PortName
        {
            get
            {
                return _serialPort.PortName;
            }
            set
            {
                _serialPort.PortName = value;
            }
        }

        //begin Observer pattern
        public delegate void EventHandler(string param);
        public EventHandler DataReceived;
        //end Observer pattern

        private void StartReading()
        {
            if (!_keepReading)
            {
                _keepReading = true;
                _readThread = new Thread(ReadPort);
                _readThread.Start();
            }
        }

        private void StopReading()
        {
            if (_keepReading)
            {
                _keepReading = false;
                _readThread.Join(); //block until exits
                _readThread = null;
            }
        }
        private static readonly TimeSpan waitTime = new TimeSpan(0, 0, 0, 0, 50);
        /// <summary> Get the data and pass it on. </summary>
        private void ReadPort()
        {
            while (_keepReading)
            {
                if (_serialPort.IsOpen)
                {
                    
                    try
                    {
                        DataReceived(_serialPort.ReadLine());
                    }
                    catch (TimeoutException) { }
                }
                else
                {
                    Thread.Sleep(waitTime);
                }
            }
        }

        /// <summary> Open the serial port with current settings. </summary>
        public void Open()
        {
            Close();

            try
            {
                _serialPort.Open();
                StartReading();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            
        }

        /// <summary> Close the serial port. </summary>
        public void Close()
        {
            StopReading();
            _serialPort.Close();
        }

        /// <summary> Get the status of the serial port. </summary>
        public bool IsOpen
        {
            get
            {
                return _serialPort.IsOpen;
            }
        }


        /// <summary>Send data to the serial port after appending line ending. </summary>
        /// <param name="data">An string containing the data to send. </param>
        public void Write(string data)
        {
            if (IsOpen)
            {
                _serialPort.Write(data);
            }
        }

        public void Write(byte[] buf, int offset, int count)
        {
            _serialPort.Write(buf, offset, count);
        }
    }
}
