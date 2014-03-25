using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    public static partial class Statistics
    {
        public static partial class DistributionUtility
        {
            public const double EulerMascheroniConstant = 0.577215664901532;

            /// <summary>Evaluates a gamma-function.</summary>
            /// <param name="z">Parameter.</param>
            /// <param name="p">Precision.</param>
            /// <returns>Gamma-function.</returns>
            public static double GammaFunction(double z, double p = 10000)
            {
                var s = 1.0;
                for (var n = 1.0; n < p; n++)
                    s *= (1 / (1 + z / n)) * Math.Exp(z / n);
                var eyx = Math.Exp(-z * EulerMascheroniConstant) / z;
                return eyx * s;

                //return Factorial(z - 1);
            }

            /// <summary>Evaluates a beta-function.</summary>
            /// <param name="x">Parameter.</param>
            /// <param name="y">Parameter.</param>
            /// <param name="p">Precision.</param>
            /// <returns>Beta-function.</returns>
            public static double BetaFunction(double x, double y, double p)
            {
                var s = 1.0; 
                var xpy = x + y;
                var xmy = x * y;
                for (var n = 1.0; n < p; n++)
                    s *= 1.0 / (1.0 + xmy / n / (xpy + n));
                s *= x * y / (x + y);
                return s;
            }

            /// <summary>Calculates factorial.</summary>
            /// <param name="x">X</param>
            /// <returns>X!</returns>
            public static int Factorial(int x)
            {
                if (x != 1) return x * Factorial(x - 1);
                else return 1;
            }

            /// <summary>Calculates factorial.</summary>
            /// <param name="x">X</param>
            /// <returns>X!</returns>
            public static double Factorial(double x)
            {
                if (x > 1.0) return x * Factorial(x - 1.0);
                else return 1.0;
            }

            public static int CalculateCombinationsCount(int elementsCount, int elementsPerCombination)
            {
                return Factorial(elementsCount) / Factorial(elementsPerCombination) / 
                    Factorial(elementsCount - elementsPerCombination);
            }

        }
    }
}
