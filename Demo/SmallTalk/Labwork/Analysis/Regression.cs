using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    using Les = Utility.LesGaussSolver;

    public static partial class Statistics
    {
        public static double[] CalculateRegressionCoefficients(
            this DataSample sampledData, 
            ref double[] errors
            )
        {
            var N = sampledData.ParametersCount;
            var M = sampledData.Items.Count;

            var A = new double[N + 1, N + 1];
            var B = new double[N + 1];

            A[0, 0] = M;

            for (var obsrv = 0; obsrv < N; obsrv++)
            {
                for (var fctrl = 0; fctrl < M; fctrl++)
                    A[0, obsrv] += sampledData[obsrv][fctrl];

                B[0] += sampledData[obsrv].Var;
                A[obsrv, 0] = A[0, obsrv];
            }

            for (var obsrvr = 0; obsrvr < N; obsrvr++)
            {
                for (var fctrl = 0; fctrl < M; fctrl++)
                {
                    for (var obsrvc = 0; obsrvc < N; obsrvc++)
                        A[obsrvr, obsrvc] += sampledData[obsrvr][fctrl] * 
                            sampledData[obsrvc][fctrl];

                    B[obsrvr + 1] += sampledData[obsrvr].Var * 
                        sampledData[obsrvr][fctrl];
                }
            }

            var lesSolver = new Les(A, B); lesSolver.Solve();
            if (errors != null) errors = lesSolver.UVector;
            return lesSolver.XVector;
        }
    }
}
