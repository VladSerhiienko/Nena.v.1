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
        public class AnovaTask : ITask<AnovaTask.Results>
        {
            public class Results
            {
                public Analysis.Statistics.Anova.Output AnovaResults;
            }

            public QuantilesLookupTask.Results FLookup;
            public Analysis.DataSample SrcSample;

            public Results Exec()
            {
                var results = new Results();
                results.AnovaResults = Analysis.Statistics.Anova.Run(SrcSample, FLookup);
                return results;
            }
        }
    }
}
