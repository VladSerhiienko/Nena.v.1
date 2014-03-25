using System;
using System.Numerics;
using System.Windows.Forms;

namespace SmallTalk.Cipher
{
    class RSA
    {
        public static BigInteger[] ResolveNumber(String inputText, String alphabet, ListBox uicontrol = null)
        {
            var result = new BigInteger[inputText.Length];

            for (int i = 0; i < inputText.Length; i++)
            {
                result[i] = alphabet.ToString().IndexOf(inputText[i]);
            }

            return result;
        }

        public static String Decrypt(BigInteger[] arrayLetter, String alphabet, ListBox uicontrol = null)
        {
            var result = String.Empty;

            try
            {
                foreach (var letter in arrayLetter)
                {
                    result += alphabet[(int)letter].ToString();
                }
            }
            catch (Exception error) 
            { 
                MessageBox.Show(
                    "Error", error.Message, 
                    MessageBoxButtons.OK, 
                    MessageBoxIcon.Error
                    ); 
            }

            return result;
        }

        public static BigInteger[] ResolveKeys(BigInteger p, BigInteger q, ListBox uicontrol = null)
        {
            var fn = BigInteger.Multiply((p - 1), (q - 1));

            ("({0}-1)({1}-1)={2}\n").ToOutputWindow(
                uicontrol, p, q, fn
                );

            var end = new BigInteger[3];

            end[0] = 17;
            end[1] = BigInteger.Multiply(p, q);
            end[2] = BigInteger.ModPow(end[0], (fn - 1), fn);

            ("[0]={0} [1=p*q]={1} [2={0}^{3}%{4}]={2}\n").ToOutputWindow(
                uicontrol, end[0], end[1], end[2], fn - 1, fn
                );


            return end;
        }

        public static BigInteger[] Encrypt(BigInteger e, BigInteger n, BigInteger[] text, ListBox uicontrol = null)
        {
            for (int i = 0; i < text.Length; i++)
            {
                text[i] = BigInteger.ModPow(text[i], e, n);
            }

            return text;
        }

    }
}
