using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class DiscreteFourierTransform
    {
        public int[] K { get; set; }
        public double[] T { get; set; }
        public double[] X { get; set; }

        public int N
        {
            get
            {
                return K.Length;
            }
        }

        public double Ak(int k)
        {
            return X[k];
        }

        public double Ckc(int k, bool trace = true)
        {
            Trace.WriteLineIf(trace, "k = " + k);
            Trace.WriteIf(trace, string.Format("C{0}c = 1/{1}(", k, N));

            double temp = 0.0F;
            for (int i = 0; i < N; i++)
            {
                double A = Ak(i);
                Trace.WriteIf(trace, "(" + A);
                double Cos = Math.Cos(2 * Math.PI * k * i / N);
                Trace.WriteIf(trace, string.Format("*cos(2*pi*{0}*{1}/{2})[{3}]", 
                    k, i, N, Math.Round(Cos, 5)));

                temp += A * Cos;

                if (i != N - 1)
                {
                    Trace.WriteIf(trace, "+");
                }
            }

            double result = temp / N;
            Trace.WriteLineIf(trace, string.Format(") = {0}", Math.Round(result, 5)));

            return result;
        }

        public double Cks(int k, bool trace = true)
        {
            Trace.WriteLineIf(trace, "k = " + k);
            Trace.WriteIf(trace, string.Format("C{0}s = 1/{1}(", k, N));

            double temp = 0.0F;
            for (int i = 0; i < N; i++)
            {
                double A = Ak(i);
                Trace.WriteIf(trace, "(" + A);
                double Sin = Math.Sin(2 * Math.PI * k * i / N);
                Trace.WriteIf(trace, string.Format("*sin(2*pi*{0}*{1}/{2})[{3}]", 
                    k, i, N, Math.Round(Sin, 5)));

                temp += A * Sin;

                if (i != N - 1)
                {
                    Trace.WriteIf(trace, "+");
                }
            }

            double result = -temp / N;
            Trace.WriteLineIf(trace, string.Format(") = {0}", Math.Round(result, 5)));

            return result;
        }

        public double Ck(int k, bool trace = true)
        {
            double Cs = Math.Round(Cks(k, false), 5);
            double Cc = Math.Round(Ckc(k, false), 5);
            double Cs2 = Math.Round(Cks(k, false) * Cks(k, false), 5);
            double Cc2 = Math.Round(Ckc(k, false) * Ckc(k, false), 5);
            double result = Math.Round(Math.Sqrt(Cks(k, false) * Cks(k, false) + Ckc(k, false) * Ckc(k, false)), 5);

            Trace.WriteLineIf(trace, string.Format(
                "C{0} = sqrt({1}^2[{2}] + {3}^2[{4}]) = {5}",
                k, Cs, Cs2, Cc, Cc2, result
                ));

            return Math.Sqrt(Cks(k, false) * Cks(k, false) + Ckc(k, false) * Ckc(k, false));
        }

        public double Phi(int k, bool trace = true)
        {
            double Cs = Cks(k, false);
            double Cc = Ckc(k, false);


            if (Math.Round(Cs, 5) == 0.0 && Math.Round(Cc, 5) == 0.0)
            {
                Trace.WriteLineIf(trace, string.Format(
                        "phi = arctg(0/0) = 0"
                        )
                    );
                return 0.0;
            }

            if (Math.Round(Cs, 5) == 0.0 || Math.Round(Cc, 5) == 0.0)
            {
                Trace.WriteLineIf(trace, string.Format(
                        "phi = arctg({0}/{1}) = pi", Math.Round(Cs, 5), Math.Round(Cc, 5)
                        )
                    );

                return Math.PI;
            }

            double value = Cs / Cc;
            double result = Math.Atan(value);

            Trace.WriteLineIf(trace, string.Format(
                    "phi = arctg({0}/{1}) =  = arctg({2}) = {3}", 
                    Math.Round(Cs, 5), Math.Round(Cs, 5), 
                    Math.Round(value, 5), 
                    Math.Round(result, 5)
                    )
                );

            return result;
        }

        public double Xk(int k)
        {

            double temp = 0.0;

            for (int i = 0; i < N; i++)
            {
                temp += Math.Abs(Ck(k, false)) * Math.Cos(2 * Math.PI * k * i / N + Phi(k, false));
            }

            return temp;
        }

        public void Demo1()
        {
            Trace.WriteLine("Demo #1");

            foreach (int i in K)
            {
                Trace.WriteLine(Ckc(i));
                Trace.WriteLine(Cks(i));
                Trace.WriteLine(Ck(i));
                Trace.WriteLine(Phi(i));
                Trace.WriteLine("\n");
            }
        }

        public void Demo2()
        {
            Trace.WriteLine("Demo #2");
            foreach (int i in K)
            {
                Trace.WriteLine(Xk(i));
            }

            Trace.WriteLine("\n");
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Start(args);

            DiscreteFourierTransform dft = new DiscreteFourierTransform() 
            { 
                K = new int[]
                {
                    0, 1, 2, 3
                },
                T = new double[]
                {
                    0, Math.PI / 4, 2 * Math.PI / 4, 3 * Math.PI / 4
                },
                X = new double[]
                {
                    0, 3, 6, 3
                    //0, 0.5, 1, 0.5
                }
            };

            dft.Demo1();
            dft.Demo2();

            Exit();
        }

        static void Start(string[] args)
        {
            Trace.Listeners.Clear();

            TextWriterTraceListener twtl = new TextWriterTraceListener("output.txt");
            twtl.Name = "Air";
            twtl.TraceOutputOptions = TraceOptions.DateTime | TraceOptions.ThreadId;

            ConsoleTraceListener ctl = new ConsoleTraceListener(false);
            ctl.TraceOutputOptions = TraceOptions.DateTime;

            Trace.Listeners.Add(ctl);
            Trace.Listeners.Add(twtl);
            Trace.AutoFlush = true;

            Trace.WriteLine("---------------------------------------------------------------------");
        }

        static void Exit()
        {
            Console.WriteLine("\nPress any key to exit ...");
            Console.ReadKey();
        }
    }
}
