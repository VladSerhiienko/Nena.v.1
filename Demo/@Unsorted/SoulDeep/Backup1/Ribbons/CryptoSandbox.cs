using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SmallTalk.FlowControl;

namespace SmallTalk.Ribbons
{
    using Cipher;
    using SmallTalk.Analysis;
    using System.IO;
    using System.Numerics;

    public partial class CryptoSandbox : Form
    {
        BigInteger[] InterstateKeys;
        BigInteger[] EndcodedKeys;

        public ListBox UI = null;

        public CryptoSandbox()
        {
            InitializeComponent();

            KeyValuePairGridView.Rows.Add("Abc", "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,;'");

            KeyValuePairGridView.Rows.Add("Rounds", "9");
            KeyValuePairGridView.Rows.Add("HillKey9", "SERHIENKO");
            KeyValuePairGridView.Rows.Add("DESKey8", "VLADYSLA");
            KeyValuePairGridView.Rows.Add("DESIv8", "SERHIENK");
            KeyValuePairGridView.Rows.Add("RSAQ", "113");
            KeyValuePairGridView.Rows.Add("RSAP", "73");

            KeyValuePairGridView.Rows.Add("Cipher (1)", "Hill");
            KeyValuePairGridView.Rows.Add("Cipher (2)", "RSA");
            KeyValuePairGridView.Rows.Add("Cipher (3)", "DES");
            KeyValuePairGridView.Rows.Add("Cipher (4)", "Feistel");
            KeyValuePairGridView.Rows.Add("Cipher", "Feistel");

            KeyValuePairGridView.Rows.Add("DESEncA2g", "DESEncA2g1");
            KeyValuePairGridView.Rows.Add("DESDecA2g", "DESDecA2g1");
            KeyValuePairGridView.Rows.Add("DESEncA3g", "DESEncA3g1");
            KeyValuePairGridView.Rows.Add("DESDecA3g", "DESDecA3g1");
            KeyValuePairGridView.Rows.Add("DESEncAa", "DESEncAa1");
            KeyValuePairGridView.Rows.Add("DESDecAa", "DESDecAa1");

            KeyValuePairGridView.Rows.Add("RSAEncA2g", "RSAEncA2g1");
            KeyValuePairGridView.Rows.Add("RSADecA2g", "RSADecA2g1");
            KeyValuePairGridView.Rows.Add("RSAEncA3g", "RSAEncA3g1");
            KeyValuePairGridView.Rows.Add("RSADecA3g", "RSADecA3g1");
            KeyValuePairGridView.Rows.Add("RSAEncAa", "RSAEncAa1");
            KeyValuePairGridView.Rows.Add("RSADecAa", "RSADecAa1");

            KeyValuePairGridView.Rows.Add("HillEncA2g", "HillEncA2g1");
            KeyValuePairGridView.Rows.Add("HillDecA2g", "HillDecA2g1");
            KeyValuePairGridView.Rows.Add("HillEncA3g", "HillEncA3g1");
            KeyValuePairGridView.Rows.Add("HillDecA3g", "HillDecA3g1");
            KeyValuePairGridView.Rows.Add("HillEncAa", "HillEncAa1");
            KeyValuePairGridView.Rows.Add("HillDecAa", "HillDecAa1");

            KeyValuePairGridView.Rows.Add("FeistelEncA2g", "FeistelEncA2g1");
            KeyValuePairGridView.Rows.Add("FeistelDecA2g", "FeistelDecA2g1");
            KeyValuePairGridView.Rows.Add("FeistelEncA3g", "FeistelEncA3g1");
            KeyValuePairGridView.Rows.Add("FeistelDecA3g", "FeistelDecA3g1");
            KeyValuePairGridView.Rows.Add("FeistelEncAa", "FeistelEncAa1");
            KeyValuePairGridView.Rows.Add("FeistelDecAa", "FeistelDecAa1");
        }

        private String GetValueByKey(String key)
        {
            foreach (DataGridViewRow row in KeyValuePairGridView.Rows)
                if ((String)row.Cells[0].Value == key && !row.Frozen) 
                    return row.Cells[1].Value as String;
            return null;
        }

        private bool SetValueByKey(String key, String value)
        {
            foreach (DataGridViewRow row in KeyValuePairGridView.Rows)
            if ((String)row.Cells[0].Value == key)
            {
                row.Cells[1].Value = value;
                return false;
            }

            KeyValuePairGridView.Rows.Add(key, value);
            return true;
        }

        private void AnalyzeDecButton_Click(object sender, EventArgs e)
        {
            var activeCipher = GetValueByKey("Cipher");
            switch (activeCipher.ToLowerInvariant())
            {
                case "feistel": AnalizeText(DecRichTextBox.Text, GetValueByKey("FeistelDecAa"), GetValueByKey("FeistelDecA2g"), GetValueByKey("FeistelDecA3g")); break;
                case "hill": AnalizeText(DecRichTextBox.Text, GetValueByKey("HillDecAa"), GetValueByKey("HillDecA2g"), GetValueByKey("HillDecA3g")); break;
                case "desx": AnalizeText(DecRichTextBox.Text, GetValueByKey("DESDecAa"), GetValueByKey("DESDecA2g"), GetValueByKey("DESDecA3g")); break;
                case "des": AnalizeText(DecRichTextBox.Text, GetValueByKey("DESDecAa"), GetValueByKey("DESDecA2g"), GetValueByKey("DESDecA3g")); break;
                case "rsa": AnalizeText(DecRichTextBox.Text, GetValueByKey("RSADecAa"), GetValueByKey("RSADecA2g"), GetValueByKey("RSADecA3g")); break;
                default: MessageBox.Show("Unsupported cipher: " + activeCipher); break;
            }

        }

        private void AnalyzeEncButton_Click(object sender, EventArgs e)
        {
            var activeCipher = GetValueByKey("Cipher");
            switch (activeCipher.ToLowerInvariant())
            {
                case "feistel": AnalizeText(EncRichTextBox.Text, GetValueByKey("FeistelEncAa"), GetValueByKey("FeistelEncA2g"), GetValueByKey("FeistelEncA3g")); break;
                case "hill": AnalizeText(EncRichTextBox.Text, GetValueByKey("HillEncAa"), GetValueByKey("HillEncA2g"), GetValueByKey("HillEncA3g")); break;
                case "rsa": AnalizeText(EncRichTextBox.Text, GetValueByKey("RSAEncAa"), GetValueByKey("RSAEncA2g"), GetValueByKey("RSAEncA3g")); break;
                case "desx": AnalizeText(EncRichTextBox.Text, GetValueByKey("DESEncAa"), null, null); break;
                case "des": AnalizeText(EncRichTextBox.Text, GetValueByKey("DESEncAa"), null, null); break;
                default: MessageBox.Show("Unsupported cipher: " + activeCipher); break;
            }
        }

        private void AnalizeText(String text, String sheet1, String sheet2, String sheet3)
        {
            AlphabetAnalyzer.Analyze(text, sheet1, UI);
            if (sheet2 != null) NGramAnalyzer.Analyze(2, text, sheet2, UI);
            if (sheet3 != null) NGramAnalyzer.Analyze(3, text, sheet3, UI);
        }

        private void EncDecButton_Click(object sender, EventArgs e)
        {
            var activeCipher = GetValueByKey("Cipher");
            switch (activeCipher.ToLowerInvariant())
            {
                case "feistel": OnEncWithFeistel(); break;
                case "hill": OnEncWithHill(); break;
                case "desx": OnEncWithDES(); break;
                case "des": OnEncWithDES(); break;
                case "rsa": OnEncWithRSA(); break;
                default: MessageBox.Show("Unsupported cipher: " + activeCipher); break;
            }
        }

        private void DecEncButton_Click(object sender, EventArgs e)
        {
            var activeCipher = GetValueByKey("Cipher");
            switch (activeCipher.ToLowerInvariant())
            {
                case "feistel": OnDecWithFeistel(); break;
                case "hill": OnDecWithHill(); break;
                case "desx": OnDecWithDES(); break;
                case "des": OnDecWithDES(); break;
                case "rsa": OnDecWithRSA(); break;
                default: MessageBox.Show("Unsupported cipher: " + activeCipher); break;
            }
        }

        private void OnDecWithDES()
        {
            var key = GetValueByKey("DESKey8").Trim();
            var iv = GetValueByKey("DESIv8").Trim();
            var valid = true;

            if (key.Length < 8)
            {
                MessageBox.Show("Error: The key is two short. Terminating...");
                valid = false;
            }
            else if (key.Length > 8)
            {
                key = key.Substring(0, 8);
                if (MessageBox.Show("The key is two long. Truncated key equals " + key + ". Proceed?", "Warning",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes) valid = false;
            }

            if (iv.Length < 8)
            {
                MessageBox.Show("Error: The init vector is two short. Terminating...");
                valid = false;
            }
            else if (iv.Length > 8)
            {
                iv = iv.Substring(0, 8);
                if (MessageBox.Show("The init vector is two long. Truncated init vector equals " + iv + ". Proceed?", "Warning",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes) valid = false;
            }

            if (!valid) return;

            DecRichTextBox.Text = DES.Decrypt(key, iv, "tempdes.dat");
        }

        private void OnEncWithDES()
        {
            var key = GetValueByKey("DESKey8").Trim();
            var iv = GetValueByKey("DESIv8").Trim();
            var valid = true;

            if (key.Length < 8)
            {
                MessageBox.Show("Error: The key is two short. Terminating...");
                valid = false;
            }
            else if (key.Length > 8)
            {
                key = key.Substring(0, 8);
                if (MessageBox.Show("The key is two long. Truncated key equals " + key + ". Proceed?", "Warning",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes) valid = false;
            }

            if (iv.Length < 8)
            {
                MessageBox.Show("Error: The init vector is two short. Terminating...");
                valid = false;
            }
            else if (iv.Length > 8)
            {
                iv = iv.Substring(0, 8);
                if (MessageBox.Show("The init vector is two long. Truncated init vector equals " + iv + ". Proceed?", "Warning",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes) valid = false;
            }

            if (!valid) return;

            DES.Encrypt(DecRichTextBox.Text, key, iv, "tempdes.dat");
            EncRichTextBox.Text = File.ReadAllText("tempdes.dat");
        }

        private void OnDecWithHill()
        {
            var activeAbc = GetValueByKey("Abc").Trim();
            var len = activeAbc.Length;
            var text = EncRichTextBox.Text.Trim();
            var key = GetValueByKey("HillKey9").Trim();
            var valid = true;

            if (key.Length < 9)
            {
                MessageBox.Show("Error: The key is two short. Terminating...");
                valid = false;
            }
            else if (key.Length > 9)
            {
                key = key.Substring(0, 9);
                if (MessageBox.Show("The key is two long. Truncated key equals " + key + ". Proceed?", "Warning",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes) valid = false;
            }

            if (!valid) return;

            var prompt = CommandPromptForm.Instance;
            if (!prompt.Visible) prompt.Show();
            var box = prompt.CommandListBox;

            DecRichTextBox.Text = Hill.Process(activeAbc, text, key, true, box);
        }

        private void OnEncWithHill()
        {
            var activeAbc = GetValueByKey("Abc").Trim();
            var text = DecRichTextBox.Text.Trim();
            var key = GetValueByKey("HillKey9").Trim();
            var valid = true;

            if (key.Length < 9)
            {
                MessageBox.Show("Error: The key is two short. Terminating...");
                valid = false;
            }
            else if (key.Length > 9)
            {
                key = key.Substring(0, 9);
                if (MessageBox.Show("The key is two long. Truncated key equals " + key + ". Proceed?", "Warning",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes)
                    valid = false;
            }

            if (!valid) return;

            var prompt = CommandPromptForm.Instance;
            if (!prompt.Visible) prompt.Show();
            var box = prompt.CommandListBox;

            EncRichTextBox.Text = Hill.Process(activeAbc, text, key, false, box);
        }

        private void OnDecWithFeistel()
        {
            var nrounds = Int32.Parse(GetValueByKey("Rounds").Trim());
            var activeAbc = GetValueByKey("Abc").Trim();
            var text = EncRichTextBox.Text.Trim();

            var prompt = CommandPromptForm.Instance;
            if (!prompt.Visible) prompt.Show();
            var box = prompt.CommandListBox;

            DecRichTextBox.Text = FeistelBlocks.Process(text, nrounds, false, box);
        }

        private void OnEncWithFeistel()
        {
            var nrounds = Int32.Parse(GetValueByKey("Rounds").Trim());
            var activeAbc = GetValueByKey("Abc").Trim();
            var text = DecRichTextBox.Text.Trim();

            var prompt = CommandPromptForm.Instance;
            if (!prompt.Visible) prompt.Show();
            var box = prompt.CommandListBox;

            EncRichTextBox.Text = FeistelBlocks.Process(text, nrounds, true, box);
        }

        private void OnDecWithRSA()
        {
            var abc = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,;-'";
            if (EndcodedKeys != null)
            {
                var prompt = CommandPromptForm.Instance;
                if (!prompt.Visible) prompt.Show();
                var box = prompt.CommandListBox;

                EndcodedKeys = RSA.Encrypt(InterstateKeys[2], InterstateKeys[1], EndcodedKeys, box);
                DecRichTextBox.Text = RSA.Decrypt(EndcodedKeys, abc, box);
            }
            else
            {
                DecRichTextBox.Text = "[Error: EndcodedKeys = null.]";
            }
        }

        private void OnEncWithRSA()
        {
            var abc = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,;-'";

            if (DecRichTextBox.Text.Length != 0)
            {
                var prompt = CommandPromptForm.Instance;
                if (!prompt.Visible) prompt.Show();
                var box = prompt.CommandListBox;

                BigInteger[] text = RSA.ResolveNumber(DecRichTextBox.Text, abc);
                if (GetValueByKey("RSAP") != null && GetValueByKey("RSAQ") != null)
                {
                    BigInteger[] keys = RSA.ResolveKeys(BigInteger.Parse(GetValueByKey("RSAP")), BigInteger.Parse(GetValueByKey("RSAQ")), box);
                    InterstateKeys = RSA.ResolveKeys(BigInteger.Parse("73"), BigInteger.Parse("109"), box);

                    SetValueByKey("RSAE", keys[0].ToString());
                    SetValueByKey("RSAN", keys[1].ToString());
                    SetValueByKey("RSAD", keys[2].ToString());

                    EndcodedKeys = RSA.Encrypt(InterstateKeys[0], InterstateKeys[1], text, box);

                    string result = "";
                    foreach (var letter in EndcodedKeys)
                        result += abc[(int)letter % abc.Length].ToString();
                    EncRichTextBox.Text = result;
                }
                else
                {
                    EncRichTextBox.Text = "[Error: RSAP and/or RSAQ keys not found. Adding new ones...]";
                    KeyValuePairGridView.Rows.Add("RSAQ", "113");
                    KeyValuePairGridView.Rows.Add("RSAP", "73");
                }
            }
        }

    }
}
