using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;
using SmallTalk.FlowControl;
using SmallTalk.Ribbons;
using SmallTalk.Analysis;

namespace SmallTalk.Ribbons
{
    using Pair = NGramAnalyzer.Pair;
    public partial class CaeserSandboxForm : Form
    {
        protected class DataGridCell
        {
            public string Enc { get; set; }
            public string Dec { get; set; }
            public int Delta { get; set; }
        }

        int Min; int Max;
        List<int> IndexBuffer;
        IDictionary<char, Pair> DecAlphaDict;
        IDictionary<char, Pair> EncAlphaDict;

        public CaeserSandboxForm()
        {
            InitializeComponent();

            EncAlphaDict = new Dictionary<char, Pair>();
            DecAlphaDict = new Dictionary<char, Pair>();
        }

        private void EvaluateButton_Click(object sender, EventArgs e)
        {
            int shift = int.Parse(ShiftTxtBox.Text);
            List<char> SymbolBuffer = EncriptedRichTxtBox.Text.ToList();

            if (shift > Max) shift %= Max;
            shift = shift < Min ? Max - (Math.Abs(shift) % Max) : shift;

            for (int i = 0; i < SymbolBuffer.Count; i++)
            {
                IndexBuffer[i] += shift;
                IndexBuffer[i] %= Max;

                SymbolBuffer[i] = 
                    EncAlphaDict.ToList().Find(
                        x => x.Value.Mapping == IndexBuffer[i]
                    ).Key;
            }

            EncriptedRichTxtBox.Text = new string(SymbolBuffer.ToArray());
        }

        private void UploadButton_Click(object sender, EventArgs e)
        {
            string path = UploadPathTxtBox.Text;
            if (path.First() == '%')
            {
                path = CommandPromptForm.GetVariableValue(path);
            }

            string buffer = System.IO.File.ReadAllText(path) // ;
                .Replace("\n", "").Replace("\r", "").Replace("\t", "");

            UpdateIndexBufferFromSource(buffer);
            EncriptedRichTxtBox.Text = buffer;
        }

        private static void LoadAlpha(string sheetName, ref IDictionary<char, Pair> map)
        {
            if (sheetName.First() == '%')
            {
                sheetName = CommandPromptForm.GetVariableValue(sheetName);
            }

            Excel.Worksheet activeSheet = null;
            foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
            {
                if (sheet.Name == sheetName)
                {
                    activeSheet = sheet;
                }
            }

            if (activeSheet == null)
            {
                throw new ArgumentException("Source sheet not found");
            }

            map.Clear();

            int count = (int)activeSheet.Cells[1, 2].Value2;
            for (var row = 0; row < count; row++)
            {
                Pair value = new Pair();
                value.Mapping = (int)activeSheet.Cells[row + 3, 3].Value2;
                value.Count = (int)activeSheet.Cells[row + 3, 5].Value2;
                value.Percentage = (float)activeSheet.Cells[row + 3, 6].Value2;

                var keyStr = (string)activeSheet.Cells[row + 3, 1].Value2;
                keyStr = keyStr.Length == 0 ? "'" : keyStr;
                var key = char.Parse(keyStr);
                map.Add(key, value);
            }
        }

        private void UpdateButton_Click(object sender, EventArgs e)
        {
            try { LoadAlpha(EncSrcTxtBox.Text, ref EncAlphaDict); }
            catch { MessageBox.Show("Encrypted alphabet not found"); }
            try { LoadAlpha(DecSrcTxtBox.Text, ref DecAlphaDict); }
            catch { MessageBox.Show("Decrypted alphabet not found"); }
        }

        class SubstitutionData
        {
            public string EncColumn { get; set; }
            public string DecColumn { get; set; }

            public SubstitutionData()
            {
                EncColumn = string.Empty;
                DecColumn = string.Empty;
            }
        }    

        private void ApplyButton_Click(object sender, EventArgs e)
        {
        }

        private void UpdateIndexBufferFromSource(string buffer)
        {
            Max = Min = 0;
            IndexBuffer = new List<int>();

            foreach (var s in buffer)
            {
                IndexBuffer.Add(EncAlphaDict[s].Mapping);
            }

            Max = 31;
            Min = 0;
        }

        private void UpdateIndexBufferFromSource(RichTextBox box)
        {
            Max = Min = 0;
            string buffer = box.Text
                .Replace("\n", "")
                .Replace("\r", "")
                .Replace("\t", "");
            UpdateIndexBufferFromSource(buffer);
        }

        private void HighlightButton_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in AlphaAnalysisGridView.Rows)
            {
                string dec = (string)row.Cells[0].Value;
                string enc = (string)row.Cells[1].Value;
            }
        }

        public void FindEntries(
            string txtToSearch, int searchStart, int searchEnd, 
            RichTextBox box, out int[] starts, out int[] ends
            )
        {
            starts = null;
            ends = null;

            List<int> startResults = new List<int>();
            List<int> endResults = new List<int>();

            int caseStart = 0;
            var finds = RichTextBoxFinds.MatchCase;
            while ((caseStart = box.Find(txtToSearch, searchStart, finds)) != -1)
            {
                int caseEnd = caseStart + txtToSearch.Length;
                startResults.Add(caseStart);
                endResults.Add(caseEnd);
            }

            starts = startResults.ToArray();
            ends = endResults.ToArray();
        }

        private void CaeserSandboxForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Escape)
            {
                this.Close();
                e.Handled = true;
            }
        }

        private void CaeserSandboxForm_Activated(object sender, EventArgs e)
        {
            this.Opacity = 1.0;
        }

        private void CaeserSandboxForm_Deactivate(object sender, EventArgs e)
        {
            this.Opacity = 0.8;
        }

        private void VigenereEncryptButton_Click(object sender, EventArgs e)
        {
            Max = 31;
            Min = 0;

            UpdateIndexBufferFromSource(EncriptedRichTxtBox);
            var SymbolBuffer = EncriptedRichTxtBox.Text.ToArray();
            var key = VigenereKeyTxtBox.Text.ToArray();

            int keyIndex = 0;
            int keyLength = key.Length;
            int bufferLength = SymbolBuffer.Length;

            for (int i = 0; i < bufferLength; i++)
            {
                int shift = EncAlphaDict.ToList()
                    .Find(x => x.Key == key[keyIndex])
                    .Value.Mapping;

                IndexBuffer[i] += shift;
                IndexBuffer[i] = IndexBuffer[i] < Min 
                    ? Max - (Math.Abs(IndexBuffer[i]) % Max)
                    : IndexBuffer[i] > Max 
                    ? IndexBuffer[i] %= Max
                    : IndexBuffer[i];
                keyIndex += 1;
                keyIndex %= keyLength;

                SymbolBuffer[i] = 
                    EncAlphaDict.ToList().Find(
                        x => x.Value.Mapping == IndexBuffer[i]
                    ).Key;

            }

            DecriptedRichTxtBox.Text = new string(SymbolBuffer.ToArray());
        }

        private void AnalyzeAlphasButton_Click(object sender, EventArgs e)
        {
            AlphabetAnalyzer.Analyze(DecriptedRichTxtBox.Text, "EncVAA", null);
        }
    }
}
