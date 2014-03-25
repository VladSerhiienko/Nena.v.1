using System;
using System.Collections.Generic;
using Excel = Microsoft.Office.Interop.Excel;

namespace SmallTalk.Labwork
{
    public partial class Laborarbeit
    {
        public partial class RegressionTask : ITask<RegressionTask.Results>
        {
            public class Results
            { 
                public class L2DRegressionPerParameter
                {
                    public Excel.Worksheet LSheet;
                    public double TA, TB, Tc, Corr, TCorr;
                    public double Fr, Fc;
                    public double A, B;
                }

                public class Q2DRegressionPerParameter
                {
                    public Excel.Worksheet QSheet;
                    public double Ayx, Byx, Cyx;
                }

                public IDictionary<int, L2DRegressionPerParameter> LRFeedback;
                public IDictionary<int, Q2DRegressionPerParameter> QRFeedback;
            }

            public QuantilesLookupTask.Results QLookup;
            public Analysis.DataSample SrcSample;
            public string RPrefix;
            public int Parameter;

            public static void AddChartsForLRegression(Excel.Worksheet sheet, int parameter, int samplesSize)
            {
                var columnWidth = (sheet.Cells[1, 1] as Excel.Range).Width;

                var chartObjs = sheet.ChartObjects() as Excel.ChartObjects;
                var range = sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 3]];
                var chartObj = chartObjs.Add(columnWidth * 14, 10, 500, 500) as Excel.ChartObject;
                var chart = chartObj.Chart;

                chart.SetSourceData(range);
                chart.ChartType = Excel.XlChartType.xlXYScatter;
                chart.ApplyLayout(10, Excel.XlChartType.xlXYScatter);
                chart.ChartStyle = 245;
                parameter++;

                Excel.SeriesCollection seriesCollection = chart.SeriesCollection();

                Excel.Series seriesY = seriesCollection.Add(sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 3]]);
                seriesY.XValues = sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 1]];
                seriesY.Values = sheet.Range[sheet.Cells[1, 2], sheet.Cells[1 + samplesSize, 2]];
                seriesY.Name = "Ystatistics";

                Excel.Series seriesYm = seriesCollection.Add(sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 3]]);
                seriesYm.XValues = sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 1]];
                seriesYm.Values = sheet.Range[sheet.Cells[1, 3], sheet.Cells[1 + samplesSize, 3]];
                seriesYm.Name = "Yregression";

                chart.ChartWizard(range, 
                    Title: "Chart: LRX" + parameter.ToString(), 
                    CategoryTitle: "Xs",
                    ValueTitle: "Ys"
                    );
            }
            public static void AddChartsForQRegression(Excel.Worksheet sheet, int parameter, int samplesSize)
            {
                var columnWidth = (sheet.Cells[1, 1] as Excel.Range).Width;

                var chartObjs = sheet.ChartObjects() as Excel.ChartObjects;
                var range = sheet.Range[sheet.Cells[1, 12], sheet.Cells[1 + samplesSize, 14]];
                var chartObj = chartObjs.Add(columnWidth * 14 + 550, 10, 500, 500) as Excel.ChartObject;
                var chart = chartObj.Chart;

                chart.SetSourceData(range);
                chart.ChartType = Excel.XlChartType.xlXYScatter;
                chart.ApplyLayout(10, Excel.XlChartType.xlXYScatter);
                chart.ChartStyle = 245;
                parameter++;

                Excel.SeriesCollection seriesCollection = chart.SeriesCollection();

                Excel.Series seriesY = seriesCollection.Add(sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 3]]);
                seriesY.XValues = sheet.Range[sheet.Cells[1, 12], sheet.Cells[1 + samplesSize, 12]];
                seriesY.Values = sheet.Range[sheet.Cells[1, 13], sheet.Cells[1 + samplesSize, 13]];
                seriesY.Name = "Ystatistics";

                Excel.Series seriesYm = seriesCollection.Add(sheet.Range[sheet.Cells[1, 1], sheet.Cells[1 + samplesSize, 3]]);
                seriesYm.XValues = sheet.Range[sheet.Cells[1, 12], sheet.Cells[1 + samplesSize, 12]];
                seriesYm.Values = sheet.Range[sheet.Cells[1, 14], sheet.Cells[1 + samplesSize, 14]];
                seriesYm.Name = "Yregression";

                chart.ChartWizard(range,
                    Title: "Chart: QRX" + parameter.ToString(),
                    CategoryTitle: "Xs",
                    ValueTitle: "Yms"
                    );
            }
            public void RunRegressionAnalysis(ref Results.L2DRegressionPerParameter rgrssn, int j)
            {
                var n = SrcSample.Items.Count;

                var xsumm = 0.0;
                var ysumm = 0.0;
                var x2summ = 0.0;
                var y2summ = 0.0;
                var xmysumm = 0.0;

                SheetSetAt(rgrssn.LSheet, 0, 0, "Xstat");
                SheetSetAt(rgrssn.LSheet, 1, 0, "Ystat");
                SheetSetAt(rgrssn.LSheet, 2, 0, "Ylreg");
                SheetSetAt(rgrssn.LSheet, 3, 0, "EXsq");
                SheetSetAt(rgrssn.LSheet, 4, 0, "EYsq");
                SheetSetAt(rgrssn.LSheet, 5, 0, "EXY");
                SheetSetAt(rgrssn.LSheet, 6, 0, "EX");
                SheetSetAt(rgrssn.LSheet, 7, 0, "EY");

                for (var i = 0; i < n; i++)
                {
                    xmysumm += SrcSample[i][j] * SrcSample[i].Var;

                    x2summ += SrcSample[i][j] * SrcSample[i][j];
                    y2summ += SrcSample[i].Var * SrcSample[i].Var;
                    xsumm += SrcSample[i][j];
                    ysumm += SrcSample[i].Var;

                    SheetSetAt(rgrssn.LSheet, 3, i + 1, x2summ);
                    SheetSetAt(rgrssn.LSheet, 4, i + 1, y2summ);
                    SheetSetAt(rgrssn.LSheet, 5, i + 1, xmysumm);
                    SheetSetAt(rgrssn.LSheet, 6, i + 1, xsumm);
                    SheetSetAt(rgrssn.LSheet, 7, i + 1, ysumm);

                }

                var T2 = n * xmysumm - xsumm * ysumm;

                var Tyx = n * x2summ - xsumm * xsumm;
                var T1yx = ysumm * x2summ - xmysumm * xsumm;

                var Txy = n * y2summ - ysumm * ysumm;
                var T1xy = xsumm * y2summ - xmysumm * ysumm;

                var Qx = Tyx / n;
                var Qy = Txy / n;
                var Qxy = T2 / n;
                var xm = xsumm / n;
                var ym = ysumm / n;

                var B1yx = Qxy / Qx;
                var B0yx = ym - B1yx * xm;

                var B1xy = Qxy / Qy;
                var B0xy = ym - B1xy * xm;

                var Qyx = Qy - B1yx * Qxy;

                var Sx = Math.Sqrt(Qx / (n - 1));
                var Sy = Math.Sqrt(Qy / (n - 1));

                var Sxy = Qxy / (n - 1);
                var Syx = Math.Sqrt(Qyx / (n - 2));

                var SB1yx = Syx / Math.Sqrt(Qx);
                var SB0yx = Syx / Math.Sqrt(1/n + xm * xm / Qx);

                var TB0yx = Math.Abs(B0yx) / SB0yx;
                var TB1yx = Math.Abs(B1yx) / SB1yx;
                var TByxc = QLookup.LookupForTQuantile(0.95, n - 2);

                var r = Qxy / Math.Sqrt(Qx * Qy);
                var Tr = r * Math.Sqrt(n - 2) / Math.Sqrt(1 - r * r);

                var ysyr2summ = 0.0;
                Func<double, double> regression = x => B0yx + B1yx * x;
                for (var i = 0; i < n; i++)
                {
                    var yr = regression(SrcSample[i][j]);
                    var y = SrcSample[i].Var;
                    var ysyr = y - yr;
                    ysyr2summ += ysyr * ysyr;

                    SheetSetAt(rgrssn.LSheet, 0, i + 1, SrcSample[i][j]);
                    SheetSetAt(rgrssn.LSheet, 1, i + 1, y);
                    SheetSetAt(rgrssn.LSheet, 2, i + 1, yr);

                }

                var S2y = Qy / (n - 1);
                var S2yr = ysyr2summ / (n - 2);

                var Fr = S2y / S2yr;
                var Fc05 = QLookup.LookupForFisherXformOf(0.05, n - 1, n - 2);
                var Fc10 = QLookup.LookupForFisherXformOf(0.10, n - 1, n - 2);
                var Fc15 = QLookup.LookupForFisherXformOf(0.15, n - 1, n - 2);
                var Fc20 = QLookup.LookupForFisherXformOf(0.20, n - 1, n - 2);

                rgrssn.A = B0yx;
                rgrssn.B = B1yx;
                rgrssn.Fr = Fr;
                rgrssn.Fc = Fc05;
                rgrssn.TA = TB0yx;
                rgrssn.TB = TB1yx;
                rgrssn.Tc = TByxc;
                rgrssn.Corr = r;
                rgrssn.TCorr = Tr;

                Logger.PushMessage(
                    "R2DX"+ Parameter + ":",
                    "Fr", rgrssn.Fr,
                    "Fc(5%)", rgrssn.Fc,
                    "Fc(10%)", Fc10,
                    "Corr", rgrssn.Corr,
                    "TCorr", rgrssn.TCorr,
                    "Fr5 >= Fc", Fr >= Fc05,
                    "Fr10 >= Fc", Fr >= Fc10,
                    "Tr >= Tc", Tr >= rgrssn.Tc
                    );

                Logger.PushMessage(
                    "L2DX" + Parameter + ":", 
                    "A", rgrssn.A,
                    "B", rgrssn.B,
                    "TA", rgrssn.TA,
                    "TB", rgrssn.TB,
                    "Tc", rgrssn.Tc,
                    "Ta >= Tc", rgrssn.TA >= rgrssn.Tc,
                    "Tb >= Tc", rgrssn.TB >= rgrssn.Tc
                    );

            }
            public void RunRegressionAnalysis(ref Results.Q2DRegressionPerParameter rgrssn, int j)
            {
                var n = SrcSample.Items.Count;

                var xsumm = 0.0;
                var x2summ = 0.0;
                var x3summ = 0.0;
                var x4summ = 0.0;

                var ysumm = 0.0;
                var xmysumm = 0.0;
                var x2mysumm = 0.0;

                SheetSetAt(rgrssn.QSheet, 8, 0, "EX3");
                SheetSetAt(rgrssn.QSheet, 9, 0, "EX4");
                SheetSetAt(rgrssn.QSheet, 10, 0, "EX2Y");
                SheetSetAt(rgrssn.QSheet, 11, 0, "Xstat");
                SheetSetAt(rgrssn.QSheet, 12, 0, "Ystat");
                SheetSetAt(rgrssn.QSheet, 13, 0, "Yqreg");


                for (var i = 0; i < n; i++)
                {
                    var y = SrcSample[i].Var;
                    var x = SrcSample[i][j];
                    var x2 = x * x;
                    var x3 = x2 * x;
                    var x4 = x3 * x;

                    var xy = x * y;
                    var x2y = x2 * y;

                    xsumm += x;
                    x2summ += x2;
                    x3summ += x3;
                    x4summ += x4;

                    ysumm += y;
                    xmysumm += x * y;
                    x2mysumm += x2 * y;

                    SheetSetAt(rgrssn.QSheet, 8, i + 1, x3summ);
                    SheetSetAt(rgrssn.QSheet, 9, i + 1, x4summ);
                    SheetSetAt(rgrssn.QSheet, 10, i + 1, x2mysumm);

                }

                double[,] A = new double[3, 3];
                double[] B = new double[3];

                A[0, 0] = n;
                A[0, 1] = xsumm;
                A[0, 2] = x2summ;
                B[0] = ysumm;

                A[1, 0] = xsumm;
                A[1, 1] = x2summ;
                A[1, 2] = x3summ;
                B[1] = xmysumm;

                A[2, 0] = x2summ;
                A[2, 1] = x3summ;
                A[2, 2] = x4summ;
                B[2] = x2mysumm;

                var gauss = new Analysis.Utility.LesGaussSolver(A, B);
                gauss.Solve();

                Func<double, double> regression = x =>
                    gauss.XVector[2] * x * x +
                    gauss.XVector[1] * x +
                    gauss.XVector[0];

                for (var i = 0; i < n; i++)
                {
                    SheetSetAt(rgrssn.QSheet, 11, i + 1, SrcSample[i][j]);
                    SheetSetAt(rgrssn.QSheet, 12, i + 1, SrcSample[i].Var);
                    SheetSetAt(rgrssn.QSheet, 13, i + 1, regression(SrcSample[i][j]));
                }

                rgrssn.Ayx = gauss.XVector[0];
                rgrssn.Byx = gauss.XVector[1];
                rgrssn.Cyx = gauss.XVector[2];

                Logger.PushMessage(
                    "Q2DX" + Parameter + ":",
                    "A", rgrssn.Ayx,
                    "B", rgrssn.Byx,
                    "C", rgrssn.Cyx
                    );

            }
            public void PushCorrelationMatrix(ref Results results)
            {
                var correlationMatrix = Analysis.Statistics.CalculateCorrelationMatrix(SrcSample, 4);
                Logger.PushMessage(correlationMatrix[0, 0], correlationMatrix[0, 1], correlationMatrix[0, 2], correlationMatrix[0, 3]);
                Logger.PushMessage(correlationMatrix[1, 0], correlationMatrix[1, 1], correlationMatrix[1, 2], correlationMatrix[1, 3]);
                Logger.PushMessage(correlationMatrix[2, 0], correlationMatrix[2, 1], correlationMatrix[2, 2], correlationMatrix[2, 3]);
                Logger.PushMessage(correlationMatrix[3, 0], correlationMatrix[3, 1], correlationMatrix[3, 2], correlationMatrix[3, 3]);
            }
            public Results Exec()
            {
                Logger.PushMessage("Info", "Regression analysis (l/q):");

                var results = new Results();
                results.LRFeedback = new Dictionary<int, Results.L2DRegressionPerParameter>();
                results.QRFeedback = new Dictionary<int, Results.Q2DRegressionPerParameter>();

                if (Parameter <= -1) for (Parameter = 0; Parameter < SrcSample.ParametersCount; Parameter++)
                    {
                        var l2dr = new Results.L2DRegressionPerParameter();
                        var q2dr = new Results.Q2DRegressionPerParameter();

                        l2dr.LSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                        l2dr.LSheet.Name = RPrefix + Parameter;
                        q2dr.QSheet = l2dr.LSheet;

                        RunRegressionAnalysis(ref l2dr, Parameter);
                        RunRegressionAnalysis(ref q2dr, Parameter);
                        results.LRFeedback.Add(Parameter, l2dr);
                        results.QRFeedback.Add(Parameter, q2dr);

                        AddChartsForLRegression(l2dr.LSheet, Parameter, SrcSample.Items.Count);
                        AddChartsForQRegression(l2dr.LSheet, Parameter, SrcSample.Items.Count);
                    }
                else
                {
                    var l2dr = new Results.L2DRegressionPerParameter();
                    var q2dr = new Results.Q2DRegressionPerParameter();

                    l2dr.LSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                    l2dr.LSheet.Name = RPrefix + Parameter;
                    q2dr.QSheet = l2dr.LSheet;

                    RunRegressionAnalysis(ref l2dr, Parameter);
                    RunRegressionAnalysis(ref q2dr, Parameter);
                    results.LRFeedback.Add(Parameter, l2dr);
                    results.QRFeedback.Add(Parameter, q2dr);

                    AddChartsForLRegression(l2dr.LSheet, Parameter, SrcSample.Items.Count);
                    AddChartsForQRegression(l2dr.LSheet, Parameter, SrcSample.Items.Count);
                }

                Logger.NewLine();

                Logger.PushMessage("Correlation matrix:");
                PushCorrelationMatrix(ref results);

                Logger.NewLine();
                return results;
            }

        }
    }
}
