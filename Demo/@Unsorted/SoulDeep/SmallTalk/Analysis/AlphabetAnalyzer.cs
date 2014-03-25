using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;
using System.Windows.Forms;

namespace SmallTalk.Analysis
{
    using Pair = NGramAnalyzer.Pair;
    using SmallTalk.FlowControl;
    using SmallTalk.Ribbons;

    class AlphabetAnalyzer : ICommandListener
    {
        static public void GenerateMapping(ref IDictionary<char, Pair> map)
        {
            map.Clear();

            int shift = 0;

            map.Add(' ', new Pair(shift++));
            map.Add('.', new Pair(shift++));
            map.Add(',', new Pair(shift++));
            map.Add(';', new Pair(shift++));
            map.Add('-', new Pair(shift++));
            map.Add('\'', new Pair(shift++));

            for (var symbol = 'A'; symbol <= 'Z'; symbol++)
            {
                Pair p = new Pair();
                p.Mapping = symbol - 'A' + shift;
                map.Add(symbol, p);
            }
        }

        static public void GenerateMapping(ref IDictionary<char, Pair> map, string srcTxt)
        {
            map.Clear();

            int shift = 0;
            foreach (var c in srcTxt)
            {
                Pair pair = null;
                if (!map.TryGetValue(c, out pair))
                {
                    map.Add(c, new Pair(shift++));
                }
            }
        }

        static public void Analyze(string srcTxt, string destSheetName, ListBox CommandListBox)
        {
            foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
            {
                if (sheet.Name == destSheetName)
                {
                    if (CommandListBox != null)
                        CommandListBox.Items.Add("Failed: Sheet name should be unique.");
                    return;
                }
            }

            IDictionary<char,Pair> map = new SortedDictionary<char, Pair>();
            GenerateMapping(ref map, srcTxt);
            //GenerateMapping(ref map);

            foreach (var c in srcTxt)
            {
                Pair pair = null;
                if (map.TryGetValue(c, out pair))
                {
                    // Inc char count
                    pair.Count += 1;
                    map[c] = pair;
                }
                else
                {
                    Debug.WriteLine("Warning: Adding new symbol=" + c.ToString());
                    pair = pair ?? new Pair();
                    pair.Count = 1;
                    map.Add(c, pair);
                }
            }

            Globals.ThisAddIn.Application.Worksheets.Add();
            Excel.Worksheet activeSheet = Globals.ThisAddIn.Application.ActiveSheet;
            activeSheet.Name = destSheetName;

            activeSheet.Cells[1, 1].Value2 = "Count";
            activeSheet.Cells[1, 2].Value2 = map.Count;
            activeSheet.Cells[2, 1].Value2 = "Special";
            activeSheet.Cells[2, 2].Value2 = "Code";
            activeSheet.Cells[2, 3].Value2 = "Mapping";
            activeSheet.Cells[2, 4].Value2 = "Char";
            activeSheet.Cells[2, 5].Value2 = "Count";
            activeSheet.Cells[2, 6].Value2 = "Freq";
            var row = 3u;

            foreach (var pair in map)
            {
                pair.Value.Percentage = (float)pair.Value.Count / (float)srcTxt.Length;

                activeSheet.Cells[row, 4].Value2 = NGramAnalyzer.GenerateSpecialDesc(pair.Key.ToString());
                activeSheet.Cells[row, 2].Value2 = (int)pair.Key;
                activeSheet.Cells[row, 3].Value2 = pair.Value.Mapping;
                activeSheet.Cells[row, 1].Value2 = pair.Key.ToString();
                activeSheet.Cells[row, 5].Value2 = pair.Value.Count;
                activeSheet.Cells[row, 6].Value2 = pair.Value.Percentage;

                row++;
            }

            var sortedMap = from item in map
                orderby item.Value.Count ascending
                select new
                {
                    Key = NGramAnalyzer.GenerateSpecialDesc(item.Key.ToString()),
                    Value = item.Value.Count
                };

            activeSheet.Cells[1, 8].Value2 = "Special";
            activeSheet.Cells[1, 9].Value2 = "Count";
            row = 2u;

            foreach (var pair in sortedMap)
            {
                activeSheet.Cells[row, 8].Value2 = pair.Key;
                activeSheet.Cells[row, 9].Value2 = pair.Value;
                row++;
            }

            var sheetChartObjs = activeSheet.ChartObjects() as Excel.ChartObjects;
            var sheetChartRangeSortedByCountAsc = activeSheet.get_Range("H1:I" + (sortedMap.Count() + 1)) as Excel.Range;
            var sheetChartObjSortedByCountAsc = sheetChartObjs.Add(440, 20, 500, sortedMap.Count() * 15) as Excel.ChartObject;
            var sheetChartSortedByCountAsc = sheetChartObjSortedByCountAsc.Chart;

            sheetChartSortedByCountAsc.SetSourceData(sheetChartRangeSortedByCountAsc);
            sheetChartSortedByCountAsc.ChartType = Excel.XlChartType.xlBarStacked;
            sheetChartSortedByCountAsc.ApplyLayout(8, Excel.XlChartType.xlBarStacked);
            sheetChartSortedByCountAsc.ChartStyle = 33;

            sheetChartSortedByCountAsc.ChartWizard(sheetChartRangeSortedByCountAsc,
                Title: "Char=(Analysis:Alphabet,Key:Count:Asc)",
                CategoryTitle: "Symbol spec",
                ValueTitle: "Entries found in input");

            var sheetChartRangeSortedByKeyAsc = activeSheet.get_Range("D2:E" + (sortedMap.Count() + 2)) as Excel.Range;
            var sheetChartObjSortedByKeyAsc = sheetChartObjs.Add(10, sortedMap.Count() * 15 + 3*15 + 50, 800, sortedMap.Count() * 9) as Excel.ChartObject;
            var sheetChartSortedByKeyAsc = sheetChartObjSortedByKeyAsc.Chart;

            sheetChartSortedByKeyAsc.SetSourceData(sheetChartRangeSortedByKeyAsc);
            sheetChartSortedByKeyAsc.ChartType = Excel.XlChartType.xlColumnStacked;
            sheetChartSortedByKeyAsc.ApplyLayout(8, Excel.XlChartType.xlColumnStacked);
            sheetChartSortedByKeyAsc.ChartStyle = 33;

            sheetChartSortedByKeyAsc.ChartWizard(sheetChartRangeSortedByKeyAsc,
                Title: "Char=(Analysis:Alphabet,Key:Spec:Asc)",
                CategoryTitle: "Symbol spec",
                ValueTitle: "Entries found in input");

            activeSheet.UsedRange.Style.HorizontalAlignment = 
                Microsoft.Office.Interop.Excel.XlHAlign.xlHAlignRight;
        }

        public void Handle(CommandPromptForm sender, ICommand cmd, int start, int end)
        {
            var paramCollection = new ICommand.IParameter[end - start];
            for (int i = start, j = 0; i < end; i++, j++) 
                paramCollection[j] = cmd[i];

            string srcPath = string.Empty, 
                destPath = string.Empty;

            if (paramCollection[0].Type.ToLowerInvariant() == "act" &&
                paramCollection[0][0].ToLowerInvariant() == "analyze")
            {
                for (var j = 1; j < paramCollection.Length; j++)
                {

                    if (paramCollection[j].Type.ToLowerInvariant() == "param" &&
                        paramCollection[j][0].ToLowerInvariant() == "srcpath")
                    {
                        srcPath = paramCollection[j][1];
                        if (srcPath.First() == '%')
                        {
                            srcPath = CommandPromptForm.GetVariableValue(srcPath);
                        }
                    }
                    else if (paramCollection[j].Type.ToLowerInvariant() == "param" &&
                        paramCollection[j][0].ToLowerInvariant() == "destpath")
                    {
                        destPath = paramCollection[j][1];
                        if (destPath.First() == '%')
                        {
                            destPath = CommandPromptForm.GetVariableValue(destPath);
                        }
                    }
                }
            }

            if (srcPath.Length == 0)
            {
                throw new ArgumentException("Invalid source file path (" + srcPath + ").");
            }
            if (destPath.Length == 0)
            {
                destPath = "AlphabeticalAnalysis";
            }

            try
            {
                var srcTxt = System.IO.File.ReadAllText(srcPath)
                    .Replace("\n", "").Replace("\r", "").Replace("\t", "");
                Analyze(srcTxt, destPath, sender.CommandListBox);
            }
            catch
            {
                throw new ArgumentException("Invalid source file path (" + srcPath + ").");
            }
        }
    }
}
