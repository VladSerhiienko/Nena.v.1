using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SmallTalk.Cipher
{
    class FeistelBlocks
    {
        static String Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,;-'";

        public static String Process(String input, Int32 nrounds, Boolean decrypt, ListBox uicontrol)
        {

            if (!decrypt)
            {
                ("Decrypting with Feistel\n").ToOutputWindow(uicontrol);
            }
            else
            {
                ("Encrypting with Feistel\n").ToOutputWindow(uicontrol);
            }

            var alphabet = Alphabet;
            var resulttxt = String.Empty;

            if (input.Length % 2 != 0) input = input + " ";
            var lttarray = new int[input.Length];

            for (var i = 0; i < input.Length; i++)
                lttarray[i] = alphabet.ToString().IndexOf(input[i]);

            lttarray.ToOutputWindow(input.Length, "letter array", uicontrol);

            ("First pair:\n").ToOutputWindow(uicontrol);
            for (var j = 0; j < input.Length; j += 2)
            {
                var round = decrypt ? nrounds : 1;

                var l = lttarray[j + 0];
                var r = lttarray[j + 1];

                ("L={1}[{4}] R={2}[{5}]\n").ToOutputWindow(uicontrol, 0, l, r, 0, alphabet[l], alphabet[r]);

                for (var i = 0; i < nrounds; i++)
                {
                    if (i < nrounds - 1)
                    {
                        var t = l;
                        var fn = BlockFunc(round, l, alphabet.Length);
                        l = r ^ fn;
                        r = t;

                        ("Fn={3} L{0}={1}[{4}] R{0}={2}[{5}]\n").ToOutputWindow(uicontrol, i, l, r, fn, alphabet[l], alphabet[r]);
                    }
                    else
                    {
                        var fn = BlockFunc(round, l, alphabet.Length);
                        r = r ^ fn;

                        ("Fn={3} L{0}={1}[{4}] R{0}={2}[{5}]\n").ToOutputWindow(uicontrol, i, l, r, fn, alphabet[l], alphabet[r]);
                    }

                    round += decrypt ? -1 : 1;
                }

                lttarray[j + 0] = l;
                lttarray[j + 1] = r;
                ("Next pair:\n").ToOutputWindow(uicontrol);
            }

            try
            {
                foreach (var ltt in lttarray)
                {
                    resulttxt += alphabet[ltt];
                }
            }
            catch
            {
                return "[Error: Inconsistent alphabet]";
            }

            return (resulttxt);
        }

        private static Int32 BlockFunc(Int32 i, Int32 rl, Int32 len)
        {
            return ((rl * i) % len);
        }
    }
}
