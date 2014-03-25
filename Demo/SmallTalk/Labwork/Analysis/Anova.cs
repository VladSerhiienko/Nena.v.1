using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    public static partial class Statistics
    {
        public static class Anova
        {
            public class Output
            {
                public double Ssa, Sse, SseEst, Sst, Msa, Mse, Mst, Fratio, Fcritical;
                public int Mdof, Edof, Tdof;
            }

            public static Output Run(DataSample sampledData, Laborarbeit.QuantilesLookupTask.Results lookup)
            {
                var output = new Output();
                var n = (int)sampledData.Items.Count;
                var k = (int)sampledData.ParametersCount;

                var s_ik_jn_yij = default(double);
                var s_ik_jn_yijpow2 = default(double);
                var s_ik_jn_yij_pow2 = default(double);

                for (var i = 0; i < k; i++)
                {
                    var s_jn_yij = default(double);
                    var ym = sampledData.CalculateSampleMean(i);

                    for (var j = 0; j < n; j++)
                    {
                        var yij = sampledData[j][i];

                        s_jn_yij += yij;
                        s_ik_jn_yij += yij;
                        s_ik_jn_yijpow2 += yij * yij;

                        var yij_ym = yij - ym;
                        output.Sse += yij_ym * yij_ym;
                    }

                    s_ik_jn_yij_pow2 += s_jn_yij * s_jn_yij;
                }

                output.Mdof = k - 1;
                output.Tdof = k * n - 1;
                output.Edof = (n - 1) * k;

                output.Sst = s_ik_jn_yijpow2 - s_ik_jn_yij * s_ik_jn_yij / k / n;
                output.Ssa = s_ik_jn_yij_pow2 / n - s_ik_jn_yij * s_ik_jn_yij / k / n;
                output.SseEst = output.Sst - output.Ssa;

                output.Msa = output.Ssa / output.Mdof;
                output.Mse = output.Sse / output.Edof;
                output.Mst = output.Sst / output.Tdof;

                output.Fratio = output.Msa / output.Mse;
                output.Fcritical = lookup.LookupForFisherXformOf(0.05, k - 1, k * (n - 1));

                return output;
            }

        }

    }
}
