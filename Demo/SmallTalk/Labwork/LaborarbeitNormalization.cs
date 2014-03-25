using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;

namespace SmallTalk.Labwork
{
    public partial class Laborarbeit
    {
        public partial class NormalizationTask
        {
            public string DstNormPrefix;

            public void Normalize(ref Results results, int param, NormalizationTask.Results.FreqAnalysis freqReference, ref NormalizationTask.Results.PerParameterNormalization norm, int depth = 1)
            {
                var correlationData = new Analysis.DataSample();
                correlationData.ParametersCount = 2;
                var k = freqReference.ClassesCount;

                for (var classIndex = 0; classIndex < k; classIndex++)
                    correlationData.Items.Add(new Analysis.DataSample.Observation(
                        new double[] { 0, classIndex, freqReference.GetAbsFor(classIndex) }
                        ));

                /// ?????
                Func<double, double> xform = xform = x => Math.Log10(x) * 500;
                if (param == 0) xform = x => Math.Log10(x);
                /// ?????

                //var correlation = Analysis.Statistics.CalculateCorrelationCoefficient(correlationData, 0, 1);
                //if (correlation < 0.0) xform = x => Math.Log10(x);
                //else xform = x => Math.Pow(x, 1.5);

                foreach (var observation in results.DstSample)
                    observation[param] = xform(observation[param]);

                RunBasicAnalysis(results, param);
                var freq = norm.NFreqAnalysis;
                RunFreqAnalysis(ref results, ref freq, param);
                norm.NFreqAnalysis = freq;

                var v = 0.0;
                var chi2 = norm.NChi2Analysis;
                var isNorm = RunChi2Analysis(ref results, ref chi2, norm.NFreqAnalysis, param, out v);
                if (!isNorm && v < 0.33 && depth < 3)
                { norm.NChi2Analysis = chi2; Normalize(ref results, param, norm.NFreqAnalysis, ref norm, depth + 1); }
                else
                { norm.NChi2Analysis = chi2; }

                

            }

        }

    }
}