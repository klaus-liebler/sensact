using System.Net;
using System.Text;
using System.Net.Sockets;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Serilog;

namespace Klli.Sensact.UdpMonitor
{


    class Binlog2Serilog
    {
        public static string ByteArrayToString(IEnumerable<byte> ba)
        {
            StringBuilder hex = new StringBuilder(ba.Count() * 2);
            foreach (byte b in ba)
                hex.AppendFormat("{0:x2}", b);
            return hex.ToString();
        }

        public void ReceiveCallback(IAsyncResult ar)
        {
            UdpClient u = (ar.AsyncState as UdpClient)!;
            IPEndPoint otherStation = new IPEndPoint(IPAddress.Any, 9876);
            byte[] receiveBytes = u.EndReceive(ar, ref otherStation!);

            string host = otherStation.Address.ToString();
            UInt32 flags = BitConverter.ToUInt32(receiveBytes, 0);
            UInt32 identifier = BitConverter.ToUInt32(receiveBytes, 4);
            byte dlc = receiveBytes[8];
            string value = $"id=0x{identifier:x2}; dlc={dlc}; payload=0x{ByteArrayToString(new ArraySegment<byte>(receiveBytes, 9, dlc))}";
            Console.WriteLine($"Received: {value}");
            cCANMessageBuilderParserOld p = new cCANMessageBuilderParserOld();
            CANMessage m = new CANMessage();
            m.Data = new byte[8];
            m.Id = identifier;
            m.DataLen = dlc;
            Array.ConstrainedCopy(receiveBytes, 9, m.Data, 0, dlc);
            p.TraceCommandMessage(m);
            u.BeginReceive(new AsyncCallback(ReceiveCallback), u);
        }

        public void InitAndRun()
        {
            IPEndPoint e = new IPEndPoint(IPAddress.Any, 9876);
            UdpClient u = new UdpClient(e);
            u.BeginReceive(new AsyncCallback(ReceiveCallback), u);
        }

        public void Close()
        {

        }
    }


    class Program
    {


        private static void ConfigureServices(IServiceCollection services)
        {
            services
                .AddLogging(configure => configure.AddSerilog())
                .AddTransient<Binlog2Serilog>();
        }

        static void Main(string[] args)
        {
            Log.Logger = new LoggerConfiguration().WriteTo.File("consoleapp.log").CreateLogger();
            var serviceCollection = new ServiceCollection();
            ConfigureServices(serviceCollection);

            var serviceProvider = serviceCollection.BuildServiceProvider();

            var logger = serviceProvider.GetService<ILogger<Program>>();



            var udplog = serviceProvider.GetService<Binlog2Serilog>()!;
            Console.WriteLine("listening for messages");
            udplog.InitAndRun();
            Console.WriteLine("Press any key to finish");
            Console.ReadKey();
            Console.WriteLine("Closing all files");
            udplog.Close();
            Console.WriteLine("Exiting");
        }
    }
}