#define CONSOLE_WINDOW_ENABLED

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LempelZivWelch;
using Common;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            Supervisor.IS_CONSOLE_WINDOW_ENABLED = true;
            Supervisor.WriteMode();

            LZWDictionary dictionary = new LZWDictionary();
            dictionary.Initialize("#ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            LZWCodec codec = new LZWCodec(dictionary);
            uint[] encoded = codec.Encode("TOBEORNOTTOBEORTOBEORNOT#");
            Supervisor.Write("Encoded message: " + LZWCodec.ToString(encoded));

            Supervisor.Write("\n\n\n\n\n\n");

            dictionary = new LZWDictionary();
            dictionary.Initialize("#ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            codec = new LZWCodec(dictionary);
            string decoded = codec.Decode(encoded);
            Supervisor.Write("Decoded message: " + decoded);

            Console.WriteLine("Press any key to exit ...");
            Console.ReadKey();
        }
    }
}
