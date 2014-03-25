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
            public static double IntegrateWithSimpsonsRule(Func<double, double> f, double a, double b)
            {
                return (b - a) * (f(a) + f(b) + 4 * f(0.5 * (b + a))) / 6.0;
            }

            public static double IntegrateWithCompositeSimpsonsRule(Func<double, double> f, double a, double b, int parts = 10000)
            {
                var s = f(a) + f(b);
                var h = (b - a) / parts;

                if (parts % 2 != 0) parts++;
                var me = parts / 2;
                var mo = me - 1;

                for (var i = 0; i < me; i += 2) s += 4 * (a + i * h);
                for (var i = 1u; i < mo; i += 2) s += 2 * (a + i * h);

                return s * h / 3;
            }

        }
    }
}
