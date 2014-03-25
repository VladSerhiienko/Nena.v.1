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
        public partial class NormalizationTask : ITask<NormalizationTask.Results>
        {
            public class Results
            {
                public class FreqAnalysis
                {
                    public Excel.Worksheet FreqAnalysisSheet;
                    public int ClassesCount;
                    public int ItemsCount;
                    public double Step;

                    public T GetAt<T>(int row, int column)
                    {
                        return (T)FreqAnalysisSheet.Cells[row + 2, column + 1].Value2;
                    }
                    public void SetAt(int row, int column, dynamic value)
                    {
                        FreqAnalysisSheet.Cells[row + 2, column + 1].Value2 = value;
                    }
                    public double GetMinFor(int classIndex)
                    {
                        return GetAt<double>(classIndex, 1);
                    }
                    public void SetMinFor(int classIndex, double value)
                    {
                        SetAt(classIndex, 1, value);
                    }
                    public double GetMaxFor(int classIndex)
                    {
                        return GetAt<double>(classIndex, 2);
                    }
                    public void SetMaxFor(int classIndex, double value)
                    {
                        SetAt(classIndex, 2, value);
                    }
                    public double GetMeanFor(int classIndex)
                    {
                        return GetAt<double>(classIndex, 3);
                    }
                    public void SetMeanFor(int classIndex, double value)
                    {
                        SetAt(classIndex, 3, value);
                    }
                    public int GetAbsFor(int classIndex)
                    {
                        return GetAt<int>(classIndex, 4);
                    }

                    public void SetAbsFor(int classIndex, int value)
                    {
                        SetAt(classIndex, 4, value);
                    }
                    public int GetRelFor(int classIndex)
                    {
                        return GetAt<int>(classIndex, 5);
                    }
                    public void SetRelFor(int classIndex, double value)
                    {
                        SetAt(classIndex, 5, value);
                    }
                    public int GetSrelFor(int classIndex)
                    {
                        return GetAt<int>(classIndex, 6);
                    }
                    public void SetSrelFor(int classIndex, double value)
                    {
                        SetAt(classIndex, 6, value);
                    }

                    public Excel.Range AbsRange
                    {
                        get { return FreqAnalysisSheet.Range[FreqAnalysisSheet.Cells[1, 5], FreqAnalysisSheet.Cells[1 + ClassesCount, 5]]; }
                    }

                    public int FreqAnalysisClassesCount
                    {
                        get { return GetAt<int>(0, 0); }
                    }

                }
                public class Chi2Analysis
                {
                    public Excel.Worksheet Chi2AnalysisSheet;
                    public int ClassesCount;

                    public T GetAt<T>(int row, int column)
                    {
                        return (T)Chi2AnalysisSheet.Cells[row + 2, column + 1].Value2;
                    }
                    public void SetAt(int row, int column, dynamic value)
                    {
                        Chi2AnalysisSheet.Cells[row + 2, column + 1].Value2 = value;
                    }

                }
                public class PerParameterNormalization
                {
                    public FreqAnalysis NFreqAnalysis;
                    public Chi2Analysis NChi2Analysis;
                }

                public Dictionary<int, FreqAnalysis> IFreqAnalysisLookup;
                public Dictionary<int, Chi2Analysis> IChi2AnalysisLookup;
                public Dictionary<int, PerParameterNormalization> NormalizationFeedback;
                public Analysis.DataSample DstSample;
            }

            public QuantilesLookupTask.Results QuantilesLookup;
            public Analysis.DataSample SrcSample;
            public int DesiredClassesCount;
            public int Parameter;

            public string DstFreqAnalysis;
            public string DstChi2Analysis;

            public void RunFreqAnalysis(ref Results results, ref Results.FreqAnalysis outputFreq, int param)
            {
                var k = outputFreq.ClassesCount = DesiredClassesCount == -1 
                    ? (int)(1 + 3.32 * Math.Log10(SrcSample.Items.Count)) 
                    : DesiredClassesCount;

                var max = SrcSample.Items.Max(x => x[param]);
                var min = SrcSample.Items.Min(x => x[param]);
                var step = outputFreq.Step = (max - min) / k;

                outputFreq.SetAt(-1, 0, "Class");
                outputFreq.SetAt(-1, 1, "Min");
                outputFreq.SetAt(-1, 2, "Max");
                outputFreq.SetAt(-1, 3, "Mean");
                outputFreq.SetAt(-1, 4, "Abs");
                outputFreq.SetAt(-1, 5, "Rel");
                outputFreq.SetAt(-1, 6, "Srel");

                var summAbs = 0;
                var totalItems = 0;
                for (var c = 0; c < k; c++)
                {
                    outputFreq.SetAt(c, 0, c);

                    var cmin = c * step + min;
                    var cmax = (c + 1) * step + min;
                    var cmean = (cmin + cmax) * 0.5;

                    outputFreq.SetMinFor(c, cmin);
                    outputFreq.SetMaxFor(c, cmax);
                    outputFreq.SetMeanFor(c, cmean);

                    var abs = 0;
                    foreach (var observation in SrcSample)
                    {
                        bool isInThisClass = observation[(int)param] <= cmax;
                        isInThisClass &= observation[(int)param] >= cmin;
                        if (isInThisClass) { abs++; totalItems++; }
                        
                    }

                    outputFreq.SetAbsFor(c, abs);
                    summAbs += abs;
                }

                outputFreq.ItemsCount = totalItems;

                var prevCumulative = 0.0;
                for (var c = 0; c < k; c++)
                {
                    var abs = (double)outputFreq.GetAbsFor(c);
                    var rel = abs / (double)summAbs;

                    outputFreq.SetSrelFor(c, prevCumulative + rel);
                    outputFreq.SetRelFor(c, rel);

                    prevCumulative += rel;
                }

                var sheet = outputFreq.FreqAnalysisSheet;
                var parameter = param;

                var absHistObjs = sheet.ChartObjects() as Excel.ChartObjects;
                var absHistObj = absHistObjs.Add(370, 10, 400, 200) as Excel.ChartObject;
                var range = outputFreq.AbsRange;
                var absHist = absHistObj.Chart;

                absHist.SetSourceData(range, Excel.XlRowCol.xlRows);
                absHist.ChartType = Excel.XlChartType.xlColumnClustered;
                absHist.ApplyLayout(10, Excel.XlChartType.xlColumnClustered);
                absHist.ChartStyle = 213;
                parameter++;

                summAbs = 1;
                var seriesCollection = absHist.SeriesCollection() as Excel.SeriesCollection;
                foreach (Excel.Series series in seriesCollection)
                    series.Name = "Class " + summAbs;

                absHist.ChartWizard(range, Title: "Chart: SampleParameter=X" + parameter.ToString(),
                    ValueTitle: "Class size (B)",
                    CategoryTitle: "Class index (N)"
                    );
            }
            public bool RunChi2Analysis(ref Results results, ref Results.Chi2Analysis outputChi2, Results.FreqAnalysis referenceFreq, int param, out double v)
            {
                outputChi2.SetAt(-1, 0, "Class");
                outputChi2.SetAt(-1, 1, "x");
                outputChi2.SetAt(-1, 2, "B");
                outputChi2.SetAt(-1, 3, "x2");
                outputChi2.SetAt(-1, 4, "Bx");
                outputChi2.SetAt(-1, 5, "Bx2");
                outputChi2.SetAt(-1, 6, "x - xm");
                outputChi2.SetAt(-1, 7, "z");
                outputChi2.SetAt(-1, 8, "f(z)");
                outputChi2.SetAt(-1, 9, "f(z)k'");
                outputChi2.SetAt(-1, 10, "E");
                outputChi2.SetAt(-1, 11, "BE");
                outputChi2.SetAt(-1, 12, "BE2");
                outputChi2.SetAt(-1, 13, "BE2E");

                var n = 0;
                var summBx = 0.0;
                var summBx2 = 0.0;
                var summB2x2 = 0.0;
                var k = referenceFreq.ClassesCount;
                for (var c = 0; c < k; c++)
                {
                    outputChi2.SetAt(c, 0, referenceFreq.GetAt<int>(c, 0));

                    var x = referenceFreq.GetMeanFor(c);
                    outputChi2.SetAt(c, 1, x);
                    outputChi2.SetAt(c, 3, x * x);

                    var B = referenceFreq.GetAbsFor(c);
                    var Bx = B * x;
                    var Bx2 = Bx * x;
                    outputChi2.SetAt(c, 2, B);
                    outputChi2.SetAt(c, 4, Bx);
                    outputChi2.SetAt(c, 5, Bx2);

                    summB2x2 += Bx2 * B;
                    summBx2 += Bx2;
                    summBx += Bx;
                    n += B;
                }

                outputChi2.SetAt(k, 2, n);
                outputChi2.SetAt(k, 4, summBx);
                outputChi2.SetAt(k, 5, summBx2);

                var chi2m = 0.0;
                var xx = summBx / n;
                var b = results.IFreqAnalysisLookup[param].Step;
                var ss = Math.Sqrt((summBx2 - (summBx * summBx) / n) / (n - 1));
                var kk = b * n / ss;

                outputChi2.SetAt(k + 1, 0, "xm");
                outputChi2.SetAt(k + 2, 0, "Sm");
                outputChi2.SetAt(k + 3, 0, "k'");
                outputChi2.SetAt(k + 4, 0, "b");
                outputChi2.SetAt(k + 1, 1, xx);
                outputChi2.SetAt(k + 2, 1, ss);
                outputChi2.SetAt(k + 3, 1, kk);
                outputChi2.SetAt(k + 4, 1, b);

                for (var c = 0; c < k; c++)
                {
                    var x = referenceFreq.GetMeanFor(c);
                    var xxx = x - xx;
                    var z = Math.Abs(xxx / ss);
                    var fz = QuantilesLookup.LookupForSNormQuantileFor(z);
                    var E = fz * kk;

                    outputChi2.SetAt(c, 6, xxx);
                    outputChi2.SetAt(c, 7, z);
                    outputChi2.SetAt(c, 8, fz);
                    outputChi2.SetAt(c, 9, E);
                    outputChi2.SetAt(c, 10, E);

                    var B = outputChi2.GetAt<int>(c, 2);
                    var BE = B - E;
                    var BE2 = BE * BE;
                    var BE2E = BE2 / E;

                    outputChi2.SetAt(c, 11, BE);
                    outputChi2.SetAt(c, 12, BE2);
                    outputChi2.SetAt(c, 13, BE2E);

                    chi2m += BE2E;
                }

                var chi2c = QuantilesLookup.LookupForCQuantileRightTail(0.1, k - 3);

                outputChi2.SetAt(k + 1, 13, chi2c);
                outputChi2.SetAt(k + 1, 12, "X2c");
                outputChi2.SetAt(k, 13, chi2m);
                outputChi2.SetAt(k, 12, "X2m");

                var isNorm = chi2m < chi2c;
                outputChi2.SetAt(k + 2, 12, "Result");
                outputChi2.SetAt(k + 2, 13, isNorm);

                if (isNorm) Logger.PushMessage(isNorm, "For X" + param + " we can accept 10% normal sample distribution hypothesis.");
                else Logger.PushMessage(isNorm, "X" + param + " should normalized first (before regression analysis).");

                v = ss / xx;
                return isNorm;
            }
            public bool RunBasicAnalysis(Results results, int param)
            {
                var n = (double)SrcSample.Items.Count;

                var g1 = Analysis.Statistics.CalculateAsymmetry(results.DstSample, param);
                var g2 = Analysis.Statistics.CalculateKurtosis(results.DstSample, param);
                var G1 = g1 * Math.Sqrt(n - 1) / n;
                var G2 = ((n + 1) * g2 + 6) * (n - 1) / (n - 2) / (n - 3);
                var SG1 = Math.Sqrt(6 * n * (n - 1) / (n - 2) / (n + 1) / (n + 3));
                var SG2 = Math.Sqrt(24 * n * (n - 1) * (n - 1) / (n - 3) / (n - 2) / (n + 3) / (n + 5));

                var G1abs = Math.Abs(G1);
                var G2abs = Math.Abs(G2);

                var isNorm = G1abs <= 3 * SG1;
                isNorm &= G2abs <= 5 * SG2;

                Logger.PushMessage(
                    isNorm, "X" + param,
                    "g1", g1, "g2", g2,
                    "G1", G1, "G2", G2,
                    "SG1", SG1, "SG2", SG2
                    );

                return isNorm;
            }

            public Results Exec()
            {
                var results = new Results();
                results.DstSample = (Analysis.DataSample)SrcSample.Clone();
                results.IFreqAnalysisLookup = new Dictionary<int, Results.FreqAnalysis>();
                results.IChi2AnalysisLookup = new Dictionary<int, Results.Chi2Analysis>();
                results.NormalizationFeedback = new Dictionary<int, Results.PerParameterNormalization>();

                Logger.PushMessage("Info", "Normal hypothesis:");

                if (Parameter == -1) for (Parameter = 0; Parameter < SrcSample.ParametersCount; Parameter++)
                {
                    if (!results.IFreqAnalysisLookup.ContainsKey(Parameter))
                        results.IFreqAnalysisLookup.Add(Parameter, new Results.FreqAnalysis());
                    else results.IFreqAnalysisLookup[Parameter] = new Results.FreqAnalysis();

                    results.IFreqAnalysisLookup[Parameter].FreqAnalysisSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                    results.IFreqAnalysisLookup[Parameter].FreqAnalysisSheet.Name = DstFreqAnalysis + "X" + Parameter;

                    var freq = results.IFreqAnalysisLookup[Parameter];
                    RunFreqAnalysis(ref results, ref freq, Parameter);
                    results.IFreqAnalysisLookup[Parameter] = freq;

                    double v = 0.0;
                    if (!RunBasicAnalysis(results, Parameter))
                    {
                        if (!results.IChi2AnalysisLookup.ContainsKey(Parameter))
                            results.IChi2AnalysisLookup.Add(Parameter, new Results.Chi2Analysis());
                        else results.IChi2AnalysisLookup[Parameter] = new Results.Chi2Analysis();
                        results.IChi2AnalysisLookup[Parameter].Chi2AnalysisSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                        results.IChi2AnalysisLookup[Parameter].Chi2AnalysisSheet.Name = DstChi2Analysis + "X" + Parameter;

                        var chi2 = results.IChi2AnalysisLookup[Parameter];
                        if (!RunChi2Analysis(ref results, ref chi2, results.IFreqAnalysisLookup[Parameter], Parameter, out v))
                        {
                            results.IChi2AnalysisLookup[Parameter] = chi2;

                            if (v > 0.33) continue;

                            var norm = new Results.PerParameterNormalization();
                            norm.NChi2Analysis = new Results.Chi2Analysis();
                            norm.NFreqAnalysis = new Results.FreqAnalysis();
                            norm.NChi2Analysis.Chi2AnalysisSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                            norm.NFreqAnalysis.FreqAnalysisSheet = Globals.ThisAddIn.Application.Worksheets.Add();
                            norm.NChi2Analysis.Chi2AnalysisSheet.Name = DstNormPrefix + DstChi2Analysis + "X" + Parameter;
                            norm.NFreqAnalysis.FreqAnalysisSheet.Name = DstNormPrefix + DstFreqAnalysis + "X" + Parameter;

                            Normalize(ref results, Parameter, results.IFreqAnalysisLookup[Parameter], ref norm);

                            if (!results.NormalizationFeedback.ContainsKey(Parameter))
                                results.NormalizationFeedback.Add(Parameter, norm);
                            else results.NormalizationFeedback[Parameter] = norm;
                        }
                        else results.IChi2AnalysisLookup[Parameter] = chi2;
                    }
                }

                Logger.NewLine();
                return results;
            }
        }
    }
}