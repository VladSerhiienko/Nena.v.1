using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    public static partial class Statistics
    {
        /// <summary>Calculate pearson's product-moment coefficient.</summary>
        /// <param name="sampledData">Sample.</param>
        /// <param name="x">First parameter index.</param>
        /// <param name="y">Second parameter index.</param>
        /// <returns>"+1" - perfect increasing linear relationship, "-1" - decreasing.</returns>
        /// <remarks>This is a normalized version of <see cref="CalculateCovarianceCoefficient"/></remarks>
        public static double CalculateCorrelationCoefficient(this DataSample sampledData, int x, int y)
        {
            var correlation = sampledData.CalculateCovarianceCoefficient(x, y);
            var sigmax = sampledData.CalculateStandardDeviation(x);
            var sigmay = sampledData.CalculateStandardDeviation(y);
            correlation /= sigmax * sigmay;
            return correlation;
        }

        /// <summary>Calculate pearson's product-moment coefficients.</summary>
        /// <param name="sampledData">Sample.</param>
        /// <param name="x">First parameter index.</param>
        /// <param name="y">Second parameter index.</param>
        /// <returns>"+1" - perfect increasing linear relationship, "-1" - decreasing.</returns>
        /// <remarks>This is a normalized version of <see cref="CalculateCovarianceCoefficient"/></remarks>
        public static double[,] CalculateCorrelationMatrix(this DataSample sampledData, int paramsCount)
        {
            var correlation = new double[paramsCount, paramsCount];

            for (var r = 0; r < paramsCount; r++)
                for (var c = 0; c < paramsCount; c++)
                    correlation[r, c] = CalculateCorrelationCoefficient(
                        sampledData, r, c
                        );
            return correlation;
        }
       
    }
}
