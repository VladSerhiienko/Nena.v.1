using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;
using SmallTalk.FlowControl;
using SmallTalk.Ribbons;
using System.Diagnostics;

namespace SmallTalk.Analysis
{
    class NGramAnalyzer : ICommandListener
    {
        public class Pair
        {
            public int Mapping;
            public int Count;
            public float Percentage;

            public Pair(int mapping = -1)
            {
                Mapping = mapping;
                Count = 0;
                Percentage = 0.0f;
            }
        }

        public NGramAnalyzer()
        {
        }

        static public void Analyze(int N, string srcTxt, string destSheetName, ListBox CommandListBox)
        {
            var map = new SortedDictionary<string, Pair>();
            for (var i = 0; i < srcTxt.Length - N; i++)
            {
                Pair pair = null;
                string gram = srcTxt.Substring(i, N);
                if (map.TryGetValue(gram, out pair))
                {
                    pair.Count += 1;
                    map[gram] = pair;
                }
                else
                {
                    pair = pair ?? new Pair();
                    pair.Count = 1;
                    map.Add(gram, pair);
                }
            }

            foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
            {
                if (sheet.Name == destSheetName)
                {
                    CommandListBox.Items.Add("Failed: Sheet name should be unique.");
                    return;
                }
            }

            Globals.ThisAddIn.Application.Worksheets.Add();
            Excel.Worksheet activeSheet = Globals.ThisAddIn.Application.ActiveSheet;
            activeSheet.Name = destSheetName;

            activeSheet.Cells[1, 1].Value2 = "Count";
            activeSheet.Cells[1, 2].Value2 = map.Count;
            activeSheet.Cells[2, 1].Value2 = "Special";
            activeSheet.Cells[2, 2].Value2 = N.ToString() + "Gram";
            activeSheet.Cells[2, 3].Value2 = "Count";
            activeSheet.Cells[2, 4].Value2 = "Freq";

            var row = 3u;

            try
            {
                foreach (var pair in map)
                {
                    pair.Value.Percentage = (float)pair.Value.Count / (float)srcTxt.Length;
                    activeSheet.Cells[row, 1].Value2 = GenerateSpecialDesc(pair.Key);
                    activeSheet.Cells[row, 2].Value2 = pair.Key;
                    activeSheet.Cells[row, 3].Value2 = pair.Value.Count;
                    activeSheet.Cells[row, 4].Value2 = pair.Value.Percentage;
                    row++;
                }
            }
            catch (System.Exception ex)
            {
                Debug.Write(ex.Message + "\n");
                return;
            	
            }

            var sortedMap = from item in map 
                orderby item.Value.Count ascending 
                select new 
                {
                    Key = GenerateSpecialDesc(item.Key),
                    Value = item.Value.Count
                };

            (activeSheet.Columns[1] as Excel.Range).ColumnWidth *= 2;
            (activeSheet.Columns[6] as Excel.Range).ColumnWidth *= 2;

            activeSheet.Cells[1, 6].Value2 = "Special";
            activeSheet.Cells[1, 7].Value2 = "Count";
            row = 2u;

            foreach (var pair in sortedMap)
            {
                activeSheet.Cells[row, 6].Value2 = pair.Key;
                activeSheet.Cells[row, 7].Value2 = pair.Value;
                row++;
            }

            var sheetChartRange = activeSheet.get_Range("F1:G"+ (sortedMap.Count() + 1)) as Excel.Range;
            var sheetChartObjs = activeSheet.ChartObjects() as Excel.ChartObjects;
            var sheetChartObj = sheetChartObjs.Add(440, 20, 500, sortedMap.Count() * 15) as Excel.ChartObject;
            var sheetChar = sheetChartObj.Chart;

            sheetChar.SetSourceData(sheetChartRange);
            sheetChar.ChartType = Excel.XlChartType.xlBarStacked;
            sheetChar.ApplyLayout(8, Excel.XlChartType.xlBarStacked);
            sheetChar.ChartStyle = 33;

            sheetChar.ChartWizard(sheetChartRange,
                Title: "Char=(Analysis:" + N + "Gram)",
                CategoryTitle: "Expression",
                ValueTitle: "Entries found in input");

            activeSheet.UsedRange.Style.HorizontalAlignment = 
                Microsoft.Office.Interop.Excel.XlHAlign.xlHAlignRight;
        }

        static public string GenerateSpecialDesc(string key)
        {
            string result = string.Empty;

            foreach (var c in key)
                switch (c)
                {
                    case '\t':  result      += "[Tab]";     break;
                    case '\r':  result      += "[Return]";  break;
                    case '\n':  result      += "[NewLine]"; break;
                    case ' ':   result      += "[Space]";   break;
                    case '\'':  result      += "[Quote]";   break;
                    case ';':   result      += "[Semi]";    break;
                    case '-':   result      += "[Dash]";    break;
                    case '.':   result      += "[Period]";  break;
                    case ',':   result      += "[Coma]";    break;

                    default:    result      += "[" + c.ToString() + "]"; break;
                }

            return result;
        }

        public void Handle(Ribbons.CommandPromptForm sender, ICommand cmd, int start, int end)
        {
            int N = 0;
            string srcPath = string.Empty;
            string destPath = string.Empty;

            var paramCollection = new ICommand.IParameter[end - start];
            for (int i = start, j = 0; i < end; i++, j++)
                paramCollection[j] = cmd[i];

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
                    else if (paramCollection[j].Type.ToLowerInvariant() == "param" &&
                        paramCollection[j][0].ToLowerInvariant() == "n")
                    {
                        var strValue = paramCollection[j][1];
                        if (strValue.First() == '%')
                        {
                            strValue = CommandPromptForm.GetVariableValue(strValue);
                        }

                        N = int.Parse(strValue);
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
                Analyze(N, srcTxt, destPath, sender.CommandListBox);
            }
            catch
            {
                throw new ArgumentException("Invalid source file path (" + srcPath + ").");
            }
        }
    }
}
