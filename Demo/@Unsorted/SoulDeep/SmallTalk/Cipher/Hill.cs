using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SmallTalk.Cipher
{
    class Hill
    {
        private static readonly Int32[,] IdentityMatrix = new Int32[,] { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

        private static Int32[,] Mul(Int32[,] a, Int32[,] b)
        {
            var mul3x3 = new int[a.GetLength(0), b.GetLength(1)];

            for (Int32 i = 0; i < a.GetLength(0); i++)
            for (Int32 j = 0; j < b.GetLength(1); j++)
            for (Int32 k = 0; k < b.GetLength(0); k++)
                mul3x3[i, j] += a[i, k] * b[k, j];

            return mul3x3;
        }

        private static Boolean IsIdentityMatrix(Int32[,] input3x3, Int32 len, ListBox uicontrol = null)
        {
            ("Checking whether 3x3 matrix is identity matrix\n").ToOutputWindow();

            var output3x3 = Inverse3x3(input3x3, len, uicontrol);
            output3x3 = Mul(output3x3, input3x3);

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    output3x3[i, j] = output3x3[i, j] % len;
                    if (output3x3[i, j] != IdentityMatrix[i, j])
                    {
                        ("It is not\n").ToOutputWindow();
                        return false;
                    }
                }
            }

            ("It is\n").ToOutputWindow();
            return true;
        }

        private static Int32[,] Inverse3x3(Int32[,] input3x3, int len, ListBox uicontrol = null)
        {
            ("Inversing 3x3 matrix\n").ToOutputWindow();

            var output3x3 = new Int32[3, 3];
            input3x3.ToOutputWindow(3, "input3x3", uicontrol);

            var det = (input3x3[0, 0] * (input3x3[1, 1] * input3x3[2, 2] - input3x3[1, 2] * input3x3[2, 1]) -
                input3x3[0, 1] * (input3x3[1, 0] * input3x3[2, 2] - input3x3[1, 2] * input3x3[2, 0]) +
                input3x3[0, 2] * (input3x3[1, 0] * input3x3[2, 1] - input3x3[1, 1] * input3x3[2, 0]));

            ("det(input3x3)={0}\n").ToOutputWindow(uicontrol, det);

            int abc = det;
            if (abc < 0) det = len - ((Math.Abs(abc)) % len);
            else det = (abc % len);
            det = Utility.CalcGreatestCommonDivisorOf(det, len);

            ("euc(input3x3)={0}\n").ToOutputWindow(uicontrol, det);

            output3x3[0, 0] = (int)Math.Pow(-1, 1 + 1) * ((input3x3[1, 1] * input3x3[2, 2]) - (input3x3[2, 1] * input3x3[1, 2]));
            output3x3[0, 1] = (int)Math.Pow(-1, 1 + 2) * ((input3x3[1, 0] * input3x3[2, 2]) - (input3x3[2, 0] * input3x3[1, 2]));
            output3x3[0, 2] = (int)Math.Pow(-1, 1 + 3) * ((input3x3[1, 0] * input3x3[2, 1]) - (input3x3[2, 0] * input3x3[1, 1]));
            output3x3[1, 0] = (int)Math.Pow(-1, 2 + 1) * ((input3x3[0, 1] * input3x3[2, 2]) - (input3x3[2, 1] * input3x3[0, 2]));
            output3x3[1, 1] = (int)Math.Pow(-1, 2 + 2) * ((input3x3[0, 0] * input3x3[2, 2]) - (input3x3[2, 0] * input3x3[0, 2]));
            output3x3[1, 2] = (int)Math.Pow(-1, 2 + 3) * ((input3x3[0, 0] * input3x3[2, 1]) - (input3x3[2, 0] * input3x3[0, 1]));
            output3x3[2, 0] = (int)Math.Pow(-1, 3 + 1) * ((input3x3[0, 1] * input3x3[1, 2]) - (input3x3[0, 2] * input3x3[1, 1]));
            output3x3[2, 1] = (int)Math.Pow(-1, 3 + 2) * ((input3x3[0, 0] * input3x3[1, 2]) - (input3x3[0, 2] * input3x3[1, 0]));
            output3x3[2, 2] = (int)Math.Pow(-1, 3 + 3) * ((input3x3[0, 0] * input3x3[1, 1]) - (input3x3[0, 1] * input3x3[1, 0]));

            output3x3.ToOutputWindow(3, "subdet3x3", uicontrol);

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (output3x3[i, j] < 0) output3x3[i, j] = len - ((Math.Abs(output3x3[i, j])) % len);
                    else output3x3[i, j] = (output3x3[i, j] % len);

                    output3x3[i, j] = ((output3x3[i, j] * det) % len);
                }
            }

            output3x3.ToOutputWindow(3, "eucsubdet3x3", uicontrol);

            int[,] transposed3x3 = new int[3, 3];
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    transposed3x3[i, j] = output3x3[j, i];
                }
            }

            output3x3.ToOutputWindow(3, "transposed3x3", uicontrol);

            output3x3 = transposed3x3;
            return output3x3;
        }

        public static String Process(String alphabet, String input, String keyText, Boolean encrypt, ListBox uicontrol = null)
        {
            if (encrypt)
            {
                ("Decrypting with Hill").ToOutputWindow(uicontrol);
            }
            else
            {
                ("Encrypting with Hill").ToOutputWindow(uicontrol);
            }

            var inputtxt = input;
            var resulttxt = String.Empty;
            var len = alphabet.Length;

            while (inputtxt.Length % 3 != 0)
            {
                inputtxt += ' ';
            }

            if (inputtxt.Length == 0)
            {
                ("[Error: Null input]\n").ToOutputWindow(uicontrol);
                return ("[Error: Null input]\n");
            }

            var resultlist = new List<int>();
            var keyarray = new Int32[3, 3];
            var numkeys = 0;

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    keyarray[i, j] = alphabet.ToString().IndexOf(keyText[numkeys]);
                    numkeys++;
                }
            }

            keyarray.ToOutputWindow(3, "key array", uicontrol);

            if (!IsIdentityMatrix(keyarray, alphabet.Length))
            {
                ("[Error: Non-invertible key matrix]\nConsider changing a key or alphabet\n").ToOutputWindow(uicontrol);
                return ("[Error: Non-invertible key matrix]\nConsider changing a key or alphabet\n");
            }

            if (encrypt == false) keyarray = Inverse3x3(keyarray, alphabet.Length, uicontrol);
            keyarray.ToOutputWindow(3, "inverse key array", uicontrol);

            for (var i = 0; i < inputtxt.Length; i += 3)
            {
                var ltt1 = alphabet.ToString().IndexOf(inputtxt[i + 0]); 
                var ltt2 = alphabet.ToString().IndexOf(inputtxt[i + 1]); 
                var ltt3 = alphabet.ToString().IndexOf(inputtxt[i + 2]);

                for (var j = 0; j < 3; j++)
                {
                    int lclresult = 0;

                    lclresult += keyarray[j, 0] * ltt1;
                    lclresult += keyarray[j, 1] * ltt2;
                    lclresult += keyarray[j, 2] * ltt3;
                    lclresult = lclresult % alphabet.Length;

                    resultlist.Add(lclresult);
                }
            }

            foreach (var letter in resultlist)
            {
                if (letter >= 0) resulttxt += alphabet[letter];
            }

            return resulttxt;
        }
    }
}
