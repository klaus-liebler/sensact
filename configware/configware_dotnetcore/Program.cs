using System;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using NLog;
using NLog.Extensions.Logging;
using Klli.Sensact.Model.Common;
namespace Klli.Sensact.Config
{
    public class AppSettings{
            public SourceCodeGeneratorOptions SourceCodeGenerator{get;set;}
        }
    class Program
    {
        private static Klli.Sensact.Model.Common.Model GetModel()
        {
            return TestModelBuilder.TwoNodesDemo();
            //return TestModelBuilder.BuildModelToTestAllPins();
            //Model model = TestModelBuilder.BuildLIBARDemo();
            //return Sensact.Model.Sattlerstrasse16.Build();
            //return Sattlerstrasse16Classic.Build();
            //return TestModelBuilder.BuildModelToTestAllAllI2CPinsAndCAN();
            //return TestModelBuilder.BuildMP3Demo();
            //return TestModelBuilder.BuildBLINDDemo();
        }

              

        private static IServiceProvider BuildServiceProvider(IConfiguration config)
        {
            
            
            ServiceCollection services = new ServiceCollection();
            services.AddSingleton<Sensact.Model.Common.Model>(GetModel());
            services.AddSingleton<SourceCodeGenerator>();
            services.AddLogging(loggingBuilder =>
               {
                   // configure Logging with NLog
                   loggingBuilder.ClearProviders();
                   loggingBuilder.SetMinimumLevel(Microsoft.Extensions.Logging.LogLevel.Trace);
                   loggingBuilder.AddNLog(config);
               });
            services.AddSingleton<AppSettings>(config.Get<AppSettings>());            
            return services.BuildServiceProvider();
        }

        

        static void Main(string[] args)
        {

            var logger = LogManager.GetCurrentClassLogger();
            try
            {
                var config = new ConfigurationBuilder()
                   .SetBasePath(System.IO.Directory.GetCurrentDirectory()) //From NuGet Package Microsoft.Extensions.Configuration.Json
                   .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
                   .Build();
                

                var servicesProvider = BuildServiceProvider(config);
                using (servicesProvider as IDisposable)
                {
                    var generator = servicesProvider.GetRequiredService<SourceCodeGenerator>();
                    var model = servicesProvider.GetRequiredService<Sensact.Model.Common.Model>();
                 
                    ModelContainer mc = new ModelContainer()
                    {
                        Model = model
                    };
                    if(!generator.CheckAndPrepare(mc))
                    {
                        Console.WriteLine("See errors and then press any key to exit");
                        Console.ReadKey();
                        return;
                    }
                    generator.DeleteBaseDirectory();
                    generator.generateAll(mc);
                    Console.WriteLine("Press ANY key to exit");
                    Console.ReadKey();
                }
            }
            catch (Exception ex)
            {
                // NLog: catch any exception and log it.
                logger.Error(ex, "Stopped program because of exception");
                throw;
            }
            finally
            {
                // Ensure to flush and stop internal timers/threads before application-exit (Avoid segmentation fault on Linux)
                LogManager.Shutdown();
            }

        }
    }
}
