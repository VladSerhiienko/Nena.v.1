using System;
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
using SmallTalk.Analysis;

namespace SmallTalk.Ribbons
{
    public partial class CommandPromptForm : Form
    {
        static private Dictionary<string, string> Variables;
        static private TextAnalyzer Analyzer;
        static private bool AllowDragAndDrop;

        static CommandPromptForm _Instance;
        static public CommandPromptForm Instance
        {
            get { return _Instance ?? new CommandPromptForm(); }
        }

        static public string GetVariableValue(string varName)
        {
            try
            {
                varName = varName.Replace("%", "");
                var value = Variables[varName];

                if (value.Contains('%'))
                {
                    value = GetVariableValue(
                        value.Replace("%", "")
                        );
                }

                return value;
            }
            catch
            {
                return string.Empty;
            }
        }

        static public void SaveVariables(string sheetName, ListBox CommandListBox)
        {
            foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
            {
                if (sheet.Name == sheetName)
                {
                    CommandListBox.Items.Add("Failed: Sheet name should be unique.");
                    return;
                }
            }

            Globals.ThisAddIn.Application.Worksheets.Add();
            Excel.Worksheet activeSheet = Globals.ThisAddIn.Application.ActiveSheet;
            activeSheet.Name = sheetName;

            uint row = 1u;
            activeSheet.Cells[row, 1].Value2 = "Count";
            activeSheet.Cells[row, 2].Value2 = Variables.Count;
            row++;

            foreach (var v in Variables)
            {
                activeSheet.Cells[row, 1].Value2 = v.Key;
                activeSheet.Cells[row, 2].Value2 = v.Value;
                row++;
            }

            CommandListBox.Items.Add("Succeeded: Pairs were saved.");
        }

        static public void LoadVariables(string sheetName, CommandPromptForm sender)
        {
            foreach (Excel.Worksheet sheet in Globals.ThisAddIn.Application.Worksheets)
            {
                if (sheet.Name == sheetName)
                {
                    
                    int row = 1;
                    int count = 0;
                    if (sheet.Cells[row, 1].Value2 == "Count")
                        count = (int)sheet.Cells[row, 2].Value2;

                    row++;
                    while ((row - 2) < count)
                    {
                        string cmd = "$ /Set ";
                        cmd += sheet.Cells[row, 1].Value2 + " ";
                        cmd += sheet.Cells[row, 2].Value2;
                        RunCommand(cmd, sender);
                        row++;
                    }

                    sender.CommandListBox.Items.Add("Succeeded: Pairs were loaded.");
                    return;
                }
            }

            sender.CommandListBox.Items.Add("Failed: Sheet not found.");
        }

        static public void PrintAllVariables(ListBox box)
        {
            box.Items.Add("Pairs: ");
            foreach (var pair in Variables)
                box.Items.Add(
                    "\tKey=" + pair.Key + 
                    " Value=" + pair.Value
                    );
        }

        static public void ClearAllVariables(ListBox box)
        {
            box.Items.Add("Cleared(" + Variables.Count.ToString() + ")"); ;
            Variables.Clear();
        }

        static CommandPromptForm()
        {
            AllowDragAndDrop = false;
            Variables = new Dictionary<string, string>();
            Analyzer = new TextAnalyzer();
        }

        public CommandPromptForm()
        {
            InitializeComponent();
            _Instance = this;
        }

        static private bool  AssignVariable(string command, CommandPromptForm sender)
        {
            ListBox CommandListBox = sender.CommandListBox;

            if (command[0] == '$')
            {
                command = command.Substring(1).Trim();
            }

            if (command.Substring(0, "/set".Length).ToLowerInvariant() == "/set")
            {
                var i = 0;
                command = command.Substring(4).Trim();
                while (i < command.Length)
                {
                    if (command[i++] == ' ')
                    {
                        break;
                    }
                }

                var key = command.Substring(0, i).Trim();
                var value = command.Substring(i, command.Length - i).Trim();

                if (value == null || value.Length == 0)
                {
                    if (Variables.ContainsKey(key))
                    {
                        CommandListBox.Items.Add(
                            "Erased: " + key + "=" +
                            Variables[key]
                        );

                        // Delete variable
                        Variables.Remove(key);
                    }
                    else
                    {
                        CommandListBox.Items.Add(
                        "Ignored: " + key
                        );
                    }

                    return true;
                }
                else
                {
                    if (value.First() == '%')
                    {
                        var v = GetVariableValue(value);

                        if (v.Length == 0)
                        {
                            CommandListBox.Items.Add(
                            "Failed: Variable Key=" + value.Substring(1) + " not found"
                            );

                            return true;
                        }
                        else
                        {
                            value = v;
                        }
                    }
                }
                try
                {
                    Variables.Add(key, value);
                    CommandListBox.Items.Add(
                    "Added: " + key + "=" + value
                    );
                }
                catch
                {
                    Variables[key] = value;
                    CommandListBox.Items.Add(
                    "Overrided: " + key + "=" + value
                    );
                }

                return true;
            }
            else if (command.Substring(0, "/exe".Length).ToLowerInvariant() == "/exe")
            {
                command = command.Substring("/exe".Length).Trim();

                if (command.First() == '%')
                {
                    command = GetVariableValue(command);
                }

                if (command.ToLowerInvariant() == "/allow")
                {
                    AllowDragAndDrop = true;
                    return true;
                }
                else if (command.ToLowerInvariant() == "/discard")
                {
                    AllowDragAndDrop = false;
                    return true;
                }

                var cmds = System.IO.File.ReadAllLines(command);
                foreach (var cmd in cmds)
                {
                    if (cmd.Length != 0) RunCommand(cmd, sender);
                }

                return true;
            }
            else if (command.Substring(0, "/clc".Length).ToLowerInvariant() == "/clc")
            {
                CommandListBox.Items.Clear();
                return true;
            }
            else if (command.Substring(0, "/clr".Length).ToLowerInvariant() == "/clr")
            {
                ClearAllVariables(CommandListBox);
                return true;
            }
            else if (command.Substring(0, "/who".Length).ToLowerInvariant() == "/who")
            {
                PrintAllVariables(CommandListBox);
                return true;
            }
            else if (command.Substring(0, "/echo".Length).ToLowerInvariant() == "/echo")
            {
                command = command.Substring("/echo".Length).Trim();

                if (command.First() == '%')
                {
                    command = GetVariableValue(command);
                }

                CommandListBox.Items.Add("Echo: " + command);

                return true;
            }
            else if (command.Substring(0, "/save".Length).ToLowerInvariant() == "/save")
            {
                command = command.Substring("/save".Length).Trim();

                if (command.First() == '%')
                {
                    command = GetVariableValue(command);
                }

                SaveVariables(command, CommandListBox);
                return true;
            }
            else if (command.Substring(0, "/load".Length).ToLowerInvariant() == "/load")
            {
                command = command.Substring("/load".Length).Trim();

                if (command.First() == '%')
                {
                    command = GetVariableValue(command);
                }

                LoadVariables(command, sender);
                return true;
            }



            return false;
        }

        static private void RunCommand(string command, CommandPromptForm sender)
        {
            ListBox CommandListBox = sender.CommandListBox;
            if (AssignVariable(command.Trim(), sender))
            {
                return;
            }

            FlowControl.ICommand cmd = new FlowControl.Command();

            try
            {
                cmd.Parse(command);
                Analyzer.Handle(sender, cmd, 0, cmd.Count); 
                CommandListBox.Items.Add(
                    "Succeeded: " + cmd.ToString()
                    );
            }
            catch (System.ArgumentNullException ex)
            {
                CommandListBox.Items.Add(
                    "Failed: Message: " + ex.Message
                    );
                MessageBox.Show(
                    "Cannot process the empty command.",
                    "SmallTalk: Warning"
                    );
            }
            catch (System.Exception ex)
            {
                CommandListBox.Items.Add(
                    "Failed: Message: " + ex.Message
                    );
                CommandListBox.Items.Add(
                    "Failed: TargetSite: " + ex.TargetSite
                    );
            }
        }

        private void RunButton_Click(object sender, EventArgs e)
        {
            RunCommand(CommandTxtBox.Text, this);

            CommandTxtBox.Focus();
            CommandTxtBox.Text = "$ ";
        }

        private void RunFromSelectionButton_Click(object sender, EventArgs e)
        {
            Excel.Range range = Globals.ThisAddIn.Application.Selection;
            Excel.Range cols;

            if (range.Columns.Count > 1)
            {
                cols = range.Columns[0];
                CommandListBox.Items.Add(
                    "Warning: Ignoring other columns."
                    );
            }
            else
            {
                cols = range.Columns;
            }
            try
            {
                foreach (Excel.Range column in cols.Rows)
                {
                    RunCommand(column.Value2, this);
                }
            }
            catch (System.Exception ex)
            {
                CommandListBox.Items.Add(
                    "Failed: Message: " + ex.Message
                    );
                CommandListBox.Items.Add(
                    "Failed: TargetSite: " + ex.TargetSite
                    );
            }
            finally
            {
                CommandTxtBox.Text = "$ ";
            }
        }

        private void CommandTxtBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Enter)
            {
                RunCommand(CommandTxtBox.Text, this);
                CommandTxtBox.Text = "$ ";
                e.Handled = true;
            }
        }

        private void CommandTxtBox_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.Copy;
            }

        }

        private void CommandTxtBox_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] fileNames = (string[])e.Data.GetData(DataFormats.FileDrop);

                if (AllowDragAndDrop)
                {
                    foreach (var fileName in fileNames)
                    {
                        string command = "$ /Exe " + fileName;
                        RunCommand(command, this);
                    }
                }
                else
                {
                    CommandTxtBox.Text = "$ /Exe " + fileNames[0];
                }
            }
        }

        private void CommandPromptForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Escape)
            {
                this.Close();
                e.Handled = true;
            }
        }

        private void CommandPromptForm_Activated(object sender, EventArgs e)
        {
            this.Opacity = 1.0;
        }

        private void CommandPromptForm_Deactivate(object sender, EventArgs e)
        {
            this.Opacity = 0.7;
        }
    }
}
