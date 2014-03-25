using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SmallTalk.Cipher
{
    static class Utility
    {
        public static void ToOutputWindow(this Int32 value)
        {
            System.Diagnostics.Debug.Write(value);
        }

        public static void ToOutputWindow(this String format, ListBox uicontrol = null, params Object[] args)
        {
            var wszOutput = String.Format(format, args); // Convert

            System.Diagnostics.Debug.Write(wszOutput); // To output window 
            if (uicontrol != null) uicontrol.Items.Add(wszOutput); // To trace console
        }

        public static int CalcGreatestCommonDivisorOf(Int32 a, Int32 b)
        {
            int r1, r2, t1, t2, q, r, t, inverse, temp;

            if (a < b)
            {
                temp = b;
                b = a;
                a = temp;
            }

            r1 = a;
            r2 = b;
            t1 = 0;
            t2 = 1;

            while (r2 != 0)
            {
                q = r1 / r2;
                r = r1 % r2;
                t = t1 - (q * t2);
                t1 = t2;
                t2 = t;
                r1 = r2;
                r2 = r;
            }

            inverse = t1 % t2;

            if (inverse < 0)
            {
                inverse = inverse + t2;
            }

            return inverse;
        }

        public static void ToOutputWindow(this Int32[,] matrix, Int32 ndim, String tracekey = "Key", ListBox uicontrol = null)
        {
            ("Tracing [{0}]:\n").ToOutputWindow(uicontrol, tracekey);

            for (var row = 0; row < ndim; row++)
            {
                for (var col = 0; col < ndim; col++)
                {
                    ("[{0},{1}]={2} ").ToOutputWindow(uicontrol, row, col, matrix[row, col]);
                }

                System.Diagnostics.Debug.Write("\n");
            }
        }

        public static void ToOutputWindow(this Int32[] array, Int32 ndim, String tracekey = "Key", ListBox uicontrol = null)
        {
            ("Tracing [{0}]:\n").ToOutputWindow(uicontrol, tracekey);

            for (var row = 0; row < ndim; row++)
            {
                ("[{0}]={1} ").ToOutputWindow(uicontrol, row, array[row]);
                System.Diagnostics.Debug.Write("\n");
            }
        }

    }
}
