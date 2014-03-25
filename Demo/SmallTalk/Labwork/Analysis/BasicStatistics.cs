using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    public static partial class Statistics
    {
        /*
         * Core functionality.
         */

        /// <summary> Calculates the central moment of the sample.</summary>
        /// <param name="sampledData">Sample.</param>
        /// <param name="parameterIndex">Observation parameter.</param>
        /// <param name="moment">Moment degree.</param>
        /// <param name="norm">Normalized moment.</param>
        /// <param name="abs">Absolute moment.</param>
        /// <param name="bessel">Sample mean or standard deviation.</param>
        /// <returns>Central moment of the sample.</returns>
        public static double CalculateCentralMoment(this DataSample sampledData, int parameterIndex, double moment, bool norm = false, bool abs = false, bool bessel = false)
        {
            var momentValue = default(double);
            var mean = CalculateRawMoment(sampledData, parameterIndex, 1.0, abs);
            var size = bessel ? sampledData.Items.Count - 1 : sampledData.Items.Count;

            if (abs) foreach (var entry in sampledData) 
                momentValue += Math.Pow(
                    Math.Abs(entry[parameterIndex] - mean), 
                    moment
                    );
            else foreach (var entry in sampledData.Items) 
                momentValue += Math.Pow(
                    entry[parameterIndex] - mean,
                    moment
                    );

            momentValue /= sampledData.Items.Count;

            if (norm)
            {
                var sigma = Math.Sqrt(CalculateStandardDeviation(
                    sampledData, parameterIndex
                    ));

                sigma = Math.Pow(sigma, moment);
                momentValue /= sigma;
            }

            return momentValue;
        }

        /// <summary> Calculates the raw moment of the sample.</summary>
        /// <param name="sampledData">Sample.</param>
        /// <param name="parameterIndex">Observation parameter.</param>
        /// <param name="moment">Moment degree.</param>
        /// <param name="abs">Absolute moment.</param>
        /// <param name="bessel">Sample mean or standard deviation.</param>
        /// <returns>Raw moment of the sample.</returns>
        public static double CalculateRawMoment(this DataSample sampledData, int parameterIndex, double moment, bool abs = false, bool bessel = false)
        {
            var momentValue = default(double);

            var size = bessel ? 
                sampledData.Items.Count - 1 : 
                sampledData.Items.Count;

            if (abs) foreach (var entry in sampledData)
                momentValue += Math.Abs(entry[parameterIndex]);
            else foreach (var entry in sampledData.Items)
                momentValue += entry[parameterIndex];

            momentValue /= size;
            return momentValue;
        }

        public static double CalculateCovarianceCoefficient(this DataSample sampledData, int x, int y)
        {
            var size = sampledData.Items.Count;
            var meanx = sampledData.CalculateSampleMean(x);
            var meany = sampledData.CalculateSampleMean(y);

            var covariance = default(double);
            foreach (var entry in sampledData)
                covariance += /* E( (X-EX)(Y-EY) ) */
                    (entry[x] - meanx) *
                    (entry[y] - meany);

            covariance /= size;
            return covariance;
        }

        /*
         * Shortcuts.
         */

        public static double CalculateVarianceCoefficient(this DataSample sampledData, int parameterIndex)
        {
            return sampledData.CalculateStandardDeviation(parameterIndex) /
                sampledData.CalculateSampleMean(parameterIndex);
        }
        public static double CalculateStandardDeviation(this DataSample sampledData, int parameterIndex)
        {
            return Math.Sqrt(sampledData.CalculateCentralMoment(parameterIndex, 2));
        }
        public static double CalculateSampleMean(this DataSample sampledData, int parameterIndex)
        {
            return sampledData.CalculateRawMoment(parameterIndex, 1);
        }
        public static double CalculateAsymmetry(this DataSample sampledData, int parameterIndex)
        {
            return sampledData.CalculateCentralMoment(parameterIndex, 3, norm: true);
        }
        public static double CalculateKurtosis(this DataSample sampledData, int parameterIndex)
        {
            var normalizeMoment = sampledData.CalculateCentralMoment(parameterIndex, 4, norm: true);
            return normalizeMoment - 3.0;
        }
    }
}
