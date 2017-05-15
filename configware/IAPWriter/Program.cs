using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace IAPWriter
{
    class Program
    {
        private string port;
        private int nodeId;
        private byte[] firmware;

        public Program(string port, int nodeId, byte[] firmware)
        {
            this.port = port;
            this.nodeId = nodeId;
            this.firmware = firmware;
        }

        static void Main(string[] args)
        {
            string port = args[0];
            int nodeId = int.Parse(args[1]);
            string path = args[2];
            byte[] firmware = null;// File.ReadAllBytes(path);
            Program p = new Program(port, nodeId, firmware);
            p.Run();
            
            for(int i=0;i<100;i++)
            {
                Task.Run(() => p.Run());
            }
            Thread.Sleep(1000);
            return;
        }

        private void Run()
        {
            Console.WriteLine(Thread.CurrentThread.ManagedThreadId+"=" +Thread.CurrentThread.Name);
        }
    }
}
