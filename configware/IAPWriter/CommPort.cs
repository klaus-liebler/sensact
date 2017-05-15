using Crc32;
using System;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace Klli.Sensact.IAPWriter
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
    public interface ICommPortHandler
    {
        void MessageReceived(byte[] message);
        void ErrorOccured(string id, string description);
    }


    //Binary Message always starts with 0x01
    //then the number of payload bytes als uint16 little endian
    //then the payload bytes
    //then four bytes with CRC32-ethernet
    public sealed class CommPort
    {
        private enum MessageState
        {
            AWAIT_MESSAGESTART,
            AWAIT_SIZE_BYTE1,
            AWAIT_LAST_PAYLOAD,
            AWAIT_LAST_CRC,
        }
        SerialPort serialPort;
        volatile bool _keepReading;
        private uint payloadsize=UInt32.MaxValue;
        private MessageState state= MessageState.AWAIT_MESSAGESTART;
        private byte[] buffer = new byte[2];
        private ICommPortHandler handler;

        public CommPort(ICommPortHandler handler, string portName, int baudRate=115200)
        {
            this.handler = handler;
            serialPort = new SerialPort(portName, baudRate);
            serialPort.ReceivedBytesThreshold = 1;
            serialPort.DataReceived += SerialPort_DataReceived;
            
            // Set the read/write timeouts
            serialPort.ReadTimeout = 500;
            serialPort.WriteTimeout = 500;
        }

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if(!_keepReading)
            {
                return;
            }
            if(e.EventType == SerialData.Eof)
            {
                return;
            }
            if(serialPort.BytesToRead<serialPort.ReceivedBytesThreshold)
            {
                return;
            }
            switch (state)
            {
                case MessageState.AWAIT_MESSAGESTART:
                    serialPort.Read(buffer, 0, 1);
                    if (buffer[0] == 0x01)
                    {
                        state = MessageState.AWAIT_SIZE_BYTE1;
                        serialPort.ReceivedBytesThreshold = 1;
                        payloadsize = UInt32.MaxValue;
                    }
                    break;
                case MessageState.AWAIT_SIZE_BYTE1:
                    serialPort.Read(buffer, 0, 1);
                    state = MessageState.AWAIT_LAST_PAYLOAD;
                    payloadsize = buffer[0];
                    buffer = new byte[payloadsize];
                    serialPort.ReceivedBytesThreshold = (int)payloadsize;
                    break;
                case MessageState.AWAIT_LAST_PAYLOAD:
                    serialPort.Read(buffer, 0, (int)payloadsize);
                    state = MessageState.AWAIT_LAST_CRC;
                    serialPort.ReceivedBytesThreshold = 4;
                    break;
                case MessageState.AWAIT_LAST_CRC:
                    byte[] crc = new byte[4];
                    serialPort.Read(crc, (int)payloadsize, 4);
                    uint actualCrc = BitConverter.ToUInt32(crc, 0);
                    uint expectedCrc = Crc32Algorithm.Compute(0x4C11DB7, 0xFFFFFFFF, buffer);
                    if(actualCrc==expectedCrc)
                    {
                        Task.Run(()=>ProcessMessage(buffer));
                    }
                    buffer = new byte[2];
                    state = MessageState.AWAIT_MESSAGESTART;
                    serialPort.ReceivedBytesThreshold = 1;
                    break;

            }


        }

        private void ProcessMessage(byte[] buffer)
        {
            handler.MessageReceived(buffer);
        }

        private void StartReading()
        {
            
             _keepReading = true;
        }

        private void StopReading()
        {
           
            _keepReading = false;
               
        }
       

        /// <summary> Open the serial port with current settings. </summary>
        public void Open()
        {
            Close();
            serialPort.Open();
            StartReading();
        }

        /// <summary> Close the serial port. </summary>
        public void Close()
        {
            StopReading();
            serialPort.Close();
        }

        /// <summary> Get the status of the serial port. </summary>
        public bool IsOpen
        {
            get
            {
                return serialPort.IsOpen;
            }
        }


        /// <summary>Send data to the serial port after appending line ending. </summary>
        /// <param name="data">An string containing the data to send. </param>
        public void Write(string data)
        {
            if (IsOpen)
            {
                serialPort.Write(data);
            }
        }

        public void Write(byte[] buf, int offset, int count)
        {
            serialPort.Write(buf, offset, count);
        }
    }
}
