using Apache.NMS;
using Apache.NMS.ActiveMQ;
using Apache.NMS.ActiveMQ.Commands;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ActiveMqTopicListener
{
    public class BaseClass
    {
        public const string URI = "activemq:tcp://localhost:61616";
        public IConnectionFactory connectionFactory;
        public IConnection _connection;
        public ISession _session;

        public BaseClass()
        {
            connectionFactory = new ConnectionFactory(URI);
            if (_connection == null)
            {
                _connection = connectionFactory.CreateConnection();
                _connection.Start();
                _session = _connection.CreateSession();
            }
        }
    }

    public class Listener : BaseClass
    {
        public string[] DESTINATIONS = new string[] { "topic://APP_COMMAND", "topic://APP_EVENT", "topic://APP_STATUS", "topic://NODE_COMMAND", "topic://NODE_EVENT" };
        
        

        public void Initialize()
        {
            try
            {
                IConnectionFactory connectionFactory = new ConnectionFactory(URI);
                IConnection _connection = connectionFactory.CreateConnection();
                _connection.Start();
                ISession _session = _connection.CreateSession();
                IDestination dest = _session.GetDestination(DESTINATIONS[2]);
                using (IMessageConsumer consumer = _session.CreateConsumer(dest))
                {
                    Console.WriteLine("Listener started.");
                    Console.WriteLine("Listener created.");
                    IMessage message;
                    while (true)
                    {
                        message = consumer.Receive(TimeSpan.FromMilliseconds(200));
                       
                        if (message != null)
                        {
                            ActiveMQBytesMessage bm = message as ActiveMQBytesMessage;
                            Console.WriteLine(message.ToString());
                            Console.WriteLine("CONTENT: " + BitConverter.ToString(bm.Content));
                        }
                    }
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                Console.WriteLine("Press <ENTER> to exit.");
                Console.Read();
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Listener l = new Listener();
            l.Initialize();
            
        }
    }
}
