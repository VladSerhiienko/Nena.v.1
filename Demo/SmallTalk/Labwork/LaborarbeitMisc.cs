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
        public static T SheetGetAt<T>(Excel.Worksheet sheet, int x, int y)
        {
            return (T)sheet.Cells[y + 1, x + 1].Value2;
        }
        public static void SheetSetAt<T>(Excel.Worksheet sheet, int x, int y, T obj)
        {
            sheet.Cells[y + 1, x + 1].Value2 = obj;
        }
        public static void PasteSampleTo(Excel.Worksheet sheet, Analysis.DataSample sample)
        {
            sheet.Cells[1, 1].Value2 = sample.Items.Count;
            sheet.Cells[1, 2].Value2 = "X1";
            sheet.Cells[1, 3].Value2 = "X2";
            sheet.Cells[1, 4].Value2 = "X3";
            sheet.Cells[1, 5].Value2 = "X4";

            for (var i = 0; i < sample.Items.Count; i++)
            {
                sheet.Cells[i + 2, 1].Value2 = sample.Items[i].Var;
                sheet.Cells[i + 2, 2].Value2 = sample.Items[i][0];
                sheet.Cells[i + 2, 3].Value2 = sample.Items[i][1];
                sheet.Cells[i + 2, 4].Value2 = sample.Items[i][2];
                sheet.Cells[i + 2, 5].Value2 = sample.Items[i][3];
            }
        }
        public static void AddChartForSampleParameterAt(Excel.Worksheet sheet, int parameter, int sampleSize)
        {
            var chartObjs = sheet.ChartObjects() as Excel.ChartObjects;
            var range = sheet.Range[sheet.Cells[1, 2 + parameter], sheet.Cells[1 + sampleSize, 2 + parameter]];
            var chartObj = chartObjs.Add(250, 10 + 210 * parameter, 500, 200) as Excel.ChartObject;
            var chart = chartObj.Chart;

            chart.SetSourceData(range);
            chart.ChartType = Excel.XlChartType.xlXYScatterSmooth;
            chart.ApplyLayout(10, Excel.XlChartType.xlXYScatterSmooth);
            chart.ChartStyle = 245;
            parameter++;

            chart.ChartWizard(range, Title: "Chart: SampleParameter=X" + parameter.ToString(),
                ValueTitle: "Observation sampled value",
                CategoryTitle: "Observation index"
                );
        }
        public static void AddBasicStatistics(Excel.Worksheet sheet, Analysis.DataSample sample, int parameter = -1)
        {
            var startRow = 2 + sample.Items.Count;

            sheet.Cells[startRow + 1, 1].Value2 = "me";
            sheet.Cells[startRow + 2, 1].Value2 = "m2";
            sheet.Cells[startRow + 3, 1].Value2 = "m2b";
            sheet.Cells[startRow + 4, 1].Value2 = "m2u";
            sheet.Cells[startRow + 5, 1].Value2 = "m3";
            sheet.Cells[startRow + 6, 1].Value2 = "m4";
            sheet.Cells[startRow + 7, 1].Value2 = "v";
            sheet.Cells[startRow + 8, 1].Value2 = "g1";
            sheet.Cells[startRow + 9, 1].Value2 = "g2";

            if (parameter < 0 || parameter >= sample.ParametersCount) for (parameter = 0; parameter < sample.ParametersCount; parameter++)
            {
                sheet.Cells[startRow + 1, 2 + parameter].Value2 = Analysis.Statistics.CalculateRawMoment(sample, (int)parameter, 1);
                sheet.Cells[startRow + 2, 2 + parameter].Value2 = Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 2);
                sheet.Cells[startRow + 3, 2 + parameter].Value2 = Math.Sqrt(Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 2));
                sheet.Cells[startRow + 4, 2 + parameter].Value2 = Math.Sqrt(Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 2, bessel: true));
                sheet.Cells[startRow + 5, 2 + parameter].Value2 = Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 3);
                sheet.Cells[startRow + 6, 2 + parameter].Value2 = Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 4);
                sheet.Cells[startRow + 7, 2 + parameter].Value2 = Analysis.Statistics.CalculateVarianceCoefficient(sample, (int)parameter);
                sheet.Cells[startRow + 8, 2 + parameter].Value2 = Analysis.Statistics.CalculateAsymmetry(sample, (int)parameter);
                sheet.Cells[startRow + 9, 2 + parameter].Value2 = Analysis.Statistics.CalculateKurtosis(sample, (int)parameter);

            }
            else
            {
                sheet.Cells[startRow + 1, 2 + parameter].Value2 = Analysis.Statistics.CalculateRawMoment(sample, (int)parameter, 1);
                sheet.Cells[startRow + 2, 2 + parameter].Value2 = Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 2);
                sheet.Cells[startRow + 3, 2 + parameter].Value2 = Math.Sqrt(Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 2));
                sheet.Cells[startRow + 4, 2 + parameter].Value2 = Math.Sqrt(Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 2, bessel: true));
                sheet.Cells[startRow + 5, 2 + parameter].Value2 = Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 3);
                sheet.Cells[startRow + 6, 2 + parameter].Value2 = Analysis.Statistics.CalculateCentralMoment(sample, (int)parameter, 4);
                sheet.Cells[startRow + 7, 2 + parameter].Value2 = Analysis.Statistics.CalculateVarianceCoefficient(sample, (int)parameter);
                sheet.Cells[startRow + 8, 2 + parameter].Value2 = Analysis.Statistics.CalculateAsymmetry(sample, (int)parameter);
                sheet.Cells[startRow + 9, 2 + parameter].Value2 = Analysis.Statistics.CalculateKurtosis(sample, (int)parameter);
            }

        }

        public class Error : Exception
        {
            public enum Type
            {
                UnresolvedResourceError,
                ArithmeticError,
                UnexpectedError,
            }

            public Type ErrorType { get; protected set; }
            public string ErrorDesc { get; protected set; }

            public Error() : base() { }
            public Error(string error, Type ty = Type.UnexpectedError) : this() { ErrorDesc = error; ErrorType = ty; }
            public Error(string error, Exception inner, Type ty = Type.UnexpectedError) : base(inner.Message, inner) { ErrorDesc = error; ErrorType = ty; }
        }
        public interface ITask<TResults>
        {
            TResults Exec();
        }
        public class Air : ITask<Air.Results>
        {
            public class Results
            {
                public Excel.Worksheet Log;
                public int EntriesCount;

                public void PushMessage(params object[] msg)
                {
                    var column = 0;
                    Log.Cells[1 + EntriesCount, 1].Value2 = EntriesCount;
                    Log.Cells[1 + EntriesCount, 2].Value2 = msg.Length;
                    Log.Cells[1 + EntriesCount, 3].Value2 = DateTime.Now.ToShortDateString();
                    Log.Cells[1 + EntriesCount, 4].Value2 = DateTime.Now.ToShortTimeString();
                    foreach (var s in msg) Log.Cells[1 + EntriesCount, 5 + column++].Value2 = s.ToString();
                    EntriesCount++;
                }

                public void NewLine()
                {
                    EntriesCount++;
                }
            }

            public string LogName;

            public Results Exec()
            {
                foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
                    if (sheet.Name == LogName) return new Results { Log = sheet, EntriesCount = 0 };
                Excel.Worksheet log = Globals.ThisAddIn.Application.Worksheets.Add(); log.Name = LogName;
                return new Results { Log = log, EntriesCount = 0 };
            }
        }
        public class ImportTask : ITask<ImportTask.Results>
        {
            public class Results
            {
                public Excel.Worksheet DstSheet;
                public Analysis.DataSample Sample;
            }

            public int SrcSize;
            public int SrcTask;
            public string SrcFile;
            public string SrcTasksSheet;
            public string SrcYSheet;
            public string SrcX1Sheet;
            public string SrcX2Sheet;
            public string SrcX3Sheet;
            public string SrcX4Sheet;
            public string DstSheet;


            public Results Exec()
            {
                Excel.Application srcApp = null;
                Excel.Workbook srcWorkbook = null;

                if (SrcFile != "This")
                {
                    srcApp = new Excel.Application();
                    srcApp.Visible = false;

                    srcWorkbook = srcApp.Workbooks.Open(
                        SrcFile, 0, true, 5, "", "", true, 
                        Excel.XlPlatform.xlWindows, "\t", 
                        false, false, 0, true
                        );
                }
                else
                {
                    srcApp = Globals.ThisAddIn.Application;
                    srcWorkbook = srcApp.ActiveWorkbook;
                }

                Excel.Worksheet srcTasksSheet = srcWorkbook.Worksheets[SrcTasksSheet];

                int yTask = (int)srcTasksSheet.Cells[3 + SrcTask, 1].Value2;
                int x1Task = (int)srcTasksSheet.Cells[3 + SrcTask, 2].Value2;
                int x2Task = (int)srcTasksSheet.Cells[3 + SrcTask, 3].Value2;
                int x3Task = (int)srcTasksSheet.Cells[3 + SrcTask, 4].Value2;
                int x4Task = (int)srcTasksSheet.Cells[3 + SrcTask, 5].Value2;

                foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
                    if (sheet.Name == DstSheet) return new Results { DstSheet = sheet };

                Excel.Worksheet dstSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                dstSheet.Name = DstSheet;

                Excel.Worksheet srcYSheet = srcWorkbook.Worksheets[SrcYSheet];
                Excel.Worksheet srcX1Sheet = srcWorkbook.Worksheets[SrcX1Sheet];
                Excel.Worksheet srcX2Sheet = srcWorkbook.Worksheets[SrcX2Sheet];
                Excel.Worksheet srcX3Sheet = srcWorkbook.Worksheets[SrcX3Sheet];
                Excel.Worksheet srcX4Sheet = srcWorkbook.Worksheets[SrcX4Sheet];

                dstSheet.Cells[1, 1].Value2 = "Y";
                dstSheet.Cells[1, 2].Value2 = "X1";
                dstSheet.Cells[1, 3].Value2 = "X2";
                dstSheet.Cells[1, 4].Value2 = "X3";
                dstSheet.Cells[1, 5].Value2 = "X4";

                var results = new Results { DstSheet = dstSheet };
                results.Sample = new Analysis.DataSample();
                results.Sample.ParametersCount = 4;

                for (var i = 0; i < SrcSize; i++)
                {
                    double yValue = (double)srcYSheet.Cells[5 + i, yTask].Value2;
                    double x1Value = (double)srcX1Sheet.Cells[5 + i, x1Task].Value2;
                    double x2Value = (double)srcX2Sheet.Cells[5 + i, x2Task].Value2;
                    double x3Value = (double)srcX3Sheet.Cells[5 + i, x3Task].Value2;
                    double x4Value = (double)srcX4Sheet.Cells[5 + i, x4Task].Value2;

                    dstSheet.Cells[i + 2, 1].Value2 = yValue;
                    dstSheet.Cells[i + 2, 2].Value2 = x1Value;
                    dstSheet.Cells[i + 2, 3].Value2 = x2Value;
                    dstSheet.Cells[i + 2, 4].Value2 = x3Value;
                    dstSheet.Cells[i + 2, 5].Value2 = x4Value;

                    results.Sample.Items.Add(new Analysis.DataSample.Observation(
                        new double[] 
                        {
                            yValue, 
                            x1Value,
                            x2Value,
                            x3Value,
                            x4Value
                        }));

                }

                Logger.PushMessage("Info", "Sample imported.");
                Logger.NewLine();

                srcWorkbook.Close();

                AddChartForSampleParameterAt(dstSheet, 0, results.Sample.Items.Count);
                AddChartForSampleParameterAt(dstSheet, 1, results.Sample.Items.Count);
                AddChartForSampleParameterAt(dstSheet, 2, results.Sample.Items.Count);
                AddChartForSampleParameterAt(dstSheet, 3, results.Sample.Items.Count);
                AddBasicStatistics(dstSheet, results.Sample);

                return results;
            }

        }
        public class QuantilesLookupTask : ITask<QuantilesLookupTask.Results>
        {
            public class Results
            {
                public Excel.WorksheetFunction Functional;
                public double LookupForTQuantile(double p, int df, bool twoTailed = false)
                {
                    return twoTailed ? Functional.T_Inv_2T(p, df) : Functional.T_Inv(p, df);
                }
                public double LookupForCQuantileOneTail(double p, int df)
                {
                    return Functional.ChiInv(p, df);
                }
                public double LookupForCQuantileLeftTail(double p, int df)
                {
                    return Functional.ChiSq_Inv(p, df);
                }
                public double LookupForCQuantileRightTail(double p, int df)
                {
                    return Functional.ChiSq_Inv_RT(p, df);
                }
                public double LookupForSNormQuantileFor(double p)
                {
                    return 1.0 / Math.Sqrt(Math.PI * 2) * Math.Exp(p * p * -0.5);
                    //return Functional.Norm_S_Dist(p, false);
                }
                public double LookupForFisherXformOf(double p, double d1, double d2)
                {
                    return Functional.F_Inv_RT(p, d1, d2);
                }
            }

            public Results Exec()
            {
                return new Results { Functional = Globals.ThisAddIn.Application.WorksheetFunction };
            }
        }
    }
}