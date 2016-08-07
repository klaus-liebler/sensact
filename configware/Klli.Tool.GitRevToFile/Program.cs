using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Klli.Tool.GitRevToFile
{
    class Program
    {
        const string CODE = "/*ReplaceConfigSHA*/";
        static void Main(string[] args)
        {
            if(args.Length!=2)
            {
                throw new ArgumentException("Usage executable pathToSource pathToDestination");
            }
            string shakey = File.ReadAllLines(args[0])[0];
            string[] oldFile = File.ReadAllLines(args[1]);
            string[] newFile = new string[oldFile.Length];
            for(int lineNo = 0; lineNo<oldFile.Length; lineNo++)
            {
                string line = oldFile[lineNo];
                int start = line.IndexOf(CODE);
                if (start!=-1)
                {
                    start += CODE.Length;
                    int end = line.IndexOf(CODE, start);
                    newFile[lineNo] = line.Substring(0, start) +"\""+shakey + "\"" + line.Substring(end);
                }
                else
                {
                    newFile[lineNo] = line;
                }
            }
            File.WriteAllLines(args[1], newFile);

        }
    }
}
