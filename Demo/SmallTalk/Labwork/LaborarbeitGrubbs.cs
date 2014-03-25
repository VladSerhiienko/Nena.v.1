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
        public class OutliersExclusionTask : ITask<OutliersExclusionTask.Results>
        {
            public class Results
            {
                public Excel.Worksheet DstSheet { get; set; }
                public Analysis.DataSample SrcSample { get; set; }
                public Analysis.DataSample DstSample { get; set; }
                public List<Analysis.GrubbsTest.Exclusion> Outliers { get; set; }
            }

            public string DstSheet;
            public ImportTask.Results ImportResults;
            public QuantilesLookupTask.Results Lookup;

            public static void PushGrubbsOutputItem(Analysis.GrubbsTest.Exclusion item)
            {
                Logger.PushMessage( 
                    "Exclusion",
                    "param", item.Parameter,
                    "obsrv", item.Observation,
                    "m", item.Mean,
                    "S", item.Deviation,
                    "grubbs", item.Grubbs,
                    "t001", item.T001,
                    "t050", item.T050,
                    "t001c", item.T001Critical,
                    "tT050c", item.T050Critical
                    );
            }

            public Results Exec()
            {
                Analysis.DataSample processedDataAfter0;
                var outliers = Analysis.GrubbsTest.DetectOutliers(ImportResults.Sample, Lookup, 0, out processedDataAfter0);
                Analysis.DataSample processedDataAfter1;
                outliers.AddRange(Analysis.GrubbsTest.DetectOutliers(processedDataAfter0, Lookup, 1, out processedDataAfter1));
                Analysis.DataSample processedDataAfter2;
                outliers.AddRange(Analysis.GrubbsTest.DetectOutliers(processedDataAfter1, Lookup, 2, out processedDataAfter2));
                Analysis.DataSample processedDataAfter3;
                outliers.AddRange(Analysis.GrubbsTest.DetectOutliers(processedDataAfter2, Lookup, 3, out processedDataAfter3));

                var sheet = Globals.ThisAddIn.Application.Worksheets.Add() as Excel.Worksheet;
                sheet.Name = DstSheet;

                PasteSampleTo(sheet, processedDataAfter3);
                AddChartForSampleParameterAt(sheet, 0, processedDataAfter3.Items.Count);
                AddChartForSampleParameterAt(sheet, 1, processedDataAfter3.Items.Count);
                AddChartForSampleParameterAt(sheet, 2, processedDataAfter3.Items.Count);
                AddChartForSampleParameterAt(sheet, 3, processedDataAfter3.Items.Count);
                AddBasicStatistics(sheet, processedDataAfter3);

                Logger.PushMessage("Info", "Grubbs output:");
                foreach (var item in outliers) PushGrubbsOutputItem(item);
                Logger.NewLine();

                return new Results()
                {
                    DstSheet = sheet,
                    SrcSample = ImportResults.Sample,
                    DstSample = processedDataAfter3,
                    Outliers = outliers
                };
            }
        }
    }
}