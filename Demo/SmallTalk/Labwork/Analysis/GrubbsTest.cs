using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    public class GrubbsTest
    {
        public class Exclusion
        {
            public int Parameter;
            public int Observation;
            public double Deviation;
            public double Mean;

            public double Grubbs;
            public double T001Critical;
            public double T050Critical;
            public double T001;
            public double T050;
        }

        public static void SuggestOutlier(
            DataSample sampledData,
            int parameterIndex,
            out int observation,
            out double grubbs,
            out double mean,
            out double deviation
            )
        {
            var n = sampledData.Items.Count();
            mean = Statistics.CalculateSampleMean(sampledData, parameterIndex);
            deviation = Statistics.CalculateStandardDeviation(sampledData, parameterIndex);

            var outlier = default(int);
            var entryIndex = default(int);
            var maxDistance = default(double);
            foreach (var entry in sampledData)
            {
                var anotherDistance = Math.Abs(
                    entry[parameterIndex] - mean
                    );

                if (maxDistance < anotherDistance) 
                {
                    maxDistance = anotherDistance;
                    outlier = entryIndex;
                }

                entryIndex++;
            }

            grubbs = maxDistance / deviation;
            observation = outlier;
        }

        public static double CalculateCriticalStudentsQuantile(double t, int n)
        {
            return t * Math.Sqrt(n - 1) / Math.Sqrt(n - 2 + t * t);
        }

        public static List<Exclusion> DetectOutliers(
            DataSample sampledData, 
            Laborarbeit.QuantilesLookupTask.Results tquantiles,
            int parameterIndex, 
            out DataSample processedData
            )
        {
            var output = new List<Exclusion>();
            processedData = (DataSample)sampledData.Clone();

            while (true)
            {
                var grubbs = 0.0;
                var mean = 0.0;
                var deviation = 0.0;
                var outliear = 0;

                SuggestOutlier(
                    processedData, parameterIndex, 
                    out outliear, out grubbs, 
                    out mean, out deviation
                    );

                var outputItem = new Exclusion();
                outputItem.Mean = mean;
                outputItem.Grubbs = grubbs;
                outputItem.Deviation = deviation;
                outputItem.Observation = outliear;
                outputItem.Parameter = parameterIndex;
                outputItem.T001 = tquantiles.LookupForTQuantile(1 - 0.001, processedData.Items.Count - 2, false);
                outputItem.T050 = tquantiles.LookupForTQuantile(1 - 0.050, processedData.Items.Count - 2, false);
                outputItem.T001Critical = CalculateCriticalStudentsQuantile(outputItem.T001, (int)processedData.Items.Count);
                outputItem.T050Critical = CalculateCriticalStudentsQuantile(outputItem.T050, (int)processedData.Items.Count);

                if (grubbs > outputItem.T001Critical)
                {
                    output.Add(outputItem);
                    processedData.Items.RemoveAt((int)outliear);
                }
                else break;
            }

            System.GC.Collect();
            return output;
        }
    }
}
