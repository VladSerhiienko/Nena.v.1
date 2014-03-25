namespace SmallTalk.Ribbons
{
    partial class CaeserSandboxForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.panel4 = new System.Windows.Forms.Panel();
            this.HighlightButton = new System.Windows.Forms.Button();
            this.ApplyButton = new System.Windows.Forms.Button();
            this.AlphaAnalysisGridView = new System.Windows.Forms.DataGridView();
            this.EncColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DecColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.panel3 = new System.Windows.Forms.Panel();
            this.UpdateButton = new System.Windows.Forms.Button();
            this.DecSrcTxtBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.EncSrcTxtBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.UploadButton = new System.Windows.Forms.Button();
            this.EvaluateButton = new System.Windows.Forms.Button();
            this.UploadPathTxtBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.ShiftTxtBox = new System.Windows.Forms.TextBox();
            this.EncriptedRichTxtBox = new System.Windows.Forms.RichTextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.VigenereToolBoxPanel = new System.Windows.Forms.TableLayoutPanel();
            this.VigenereEncryptButton = new System.Windows.Forms.Button();
            this.AnalyzeAlphasButton = new System.Windows.Forms.Button();
            this.VigenereKeyTxtBox = new System.Windows.Forms.TextBox();
            this.DecriptedRichTxtBox = new System.Windows.Forms.RichTextBox();
            this.groupBox1.SuspendLayout();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AlphaAnalysisGridView)).BeginInit();
            this.panel3.SuspendLayout();
            this.panel1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.VigenereToolBoxPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.panel4);
            this.groupBox1.Controls.Add(this.panel3);
            this.groupBox1.Controls.Add(this.panel1);
            this.groupBox1.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox1.ForeColor = System.Drawing.SystemColors.Control;
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(204, 476);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Controls";
            // 
            // panel4
            // 
            this.panel4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel4.Controls.Add(this.HighlightButton);
            this.panel4.Controls.Add(this.ApplyButton);
            this.panel4.Controls.Add(this.AlphaAnalysisGridView);
            this.panel4.Location = new System.Drawing.Point(7, 190);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(189, 280);
            this.panel4.TabIndex = 5;
            // 
            // HighlightButton
            // 
            this.HighlightButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.HighlightButton.Location = new System.Drawing.Point(3, 253);
            this.HighlightButton.Name = "HighlightButton";
            this.HighlightButton.Size = new System.Drawing.Size(182, 23);
            this.HighlightButton.TabIndex = 2;
            this.HighlightButton.Text = "Highlight";
            this.HighlightButton.UseVisualStyleBackColor = true;
            this.HighlightButton.Click += new System.EventHandler(this.HighlightButton_Click);
            // 
            // ApplyButton
            // 
            this.ApplyButton.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.ApplyButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.ApplyButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.ApplyButton.Location = new System.Drawing.Point(3, 226);
            this.ApplyButton.Name = "ApplyButton";
            this.ApplyButton.Size = new System.Drawing.Size(182, 23);
            this.ApplyButton.TabIndex = 1;
            this.ApplyButton.Text = "Apply";
            this.ApplyButton.UseVisualStyleBackColor = false;
            this.ApplyButton.Click += new System.EventHandler(this.ApplyButton_Click);
            // 
            // AlphaAnalysisGridView
            // 
            this.AlphaAnalysisGridView.AllowUserToOrderColumns = true;
            this.AlphaAnalysisGridView.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.AlphaAnalysisGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.AlphaAnalysisGridView.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.AlphaAnalysisGridView.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.Raised;
            this.AlphaAnalysisGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.AlphaAnalysisGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.EncColumn,
            this.DecColumn});
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.Color.LightSkyBlue;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.AlphaAnalysisGridView.DefaultCellStyle = dataGridViewCellStyle1;
            this.AlphaAnalysisGridView.EnableHeadersVisualStyles = false;
            this.AlphaAnalysisGridView.Location = new System.Drawing.Point(3, 2);
            this.AlphaAnalysisGridView.Name = "AlphaAnalysisGridView";
            this.AlphaAnalysisGridView.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.AlphaAnalysisGridView.Size = new System.Drawing.Size(182, 216);
            this.AlphaAnalysisGridView.TabIndex = 0;
            // 
            // EncColumn
            // 
            this.EncColumn.HeaderText = "Enc";
            this.EncColumn.Name = "EncColumn";
            // 
            // DecColumn
            // 
            this.DecColumn.HeaderText = "Dec";
            this.DecColumn.Name = "DecColumn";
            // 
            // panel3
            // 
            this.panel3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel3.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel3.Controls.Add(this.UpdateButton);
            this.panel3.Controls.Add(this.DecSrcTxtBox);
            this.panel3.Controls.Add(this.label4);
            this.panel3.Controls.Add(this.EncSrcTxtBox);
            this.panel3.Controls.Add(this.label3);
            this.panel3.Location = new System.Drawing.Point(7, 92);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(189, 92);
            this.panel3.TabIndex = 4;
            // 
            // UpdateButton
            // 
            this.UpdateButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.UpdateButton.Location = new System.Drawing.Point(6, 61);
            this.UpdateButton.Name = "UpdateButton";
            this.UpdateButton.Size = new System.Drawing.Size(175, 23);
            this.UpdateButton.TabIndex = 2;
            this.UpdateButton.Text = "Update";
            this.UpdateButton.UseVisualStyleBackColor = true;
            this.UpdateButton.Click += new System.EventHandler(this.UpdateButton_Click);
            // 
            // DecSrcTxtBox
            // 
            this.DecSrcTxtBox.Location = new System.Drawing.Point(36, 35);
            this.DecSrcTxtBox.Name = "DecSrcTxtBox";
            this.DecSrcTxtBox.Size = new System.Drawing.Size(145, 20);
            this.DecSrcTxtBox.TabIndex = 1;
            this.DecSrcTxtBox.Text = "DecAlphabetAnalysis";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(3, 38);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(25, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "Dec";
            // 
            // EncSrcTxtBox
            // 
            this.EncSrcTxtBox.Location = new System.Drawing.Point(36, 9);
            this.EncSrcTxtBox.Name = "EncSrcTxtBox";
            this.EncSrcTxtBox.Size = new System.Drawing.Size(145, 20);
            this.EncSrcTxtBox.TabIndex = 1;
            this.EncSrcTxtBox.Text = "EncAlphabetAnalysis";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(3, 12);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(25, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Enc";
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.UploadButton);
            this.panel1.Controls.Add(this.EvaluateButton);
            this.panel1.Controls.Add(this.UploadPathTxtBox);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.ShiftTxtBox);
            this.panel1.Location = new System.Drawing.Point(6, 19);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(190, 67);
            this.panel1.TabIndex = 1;
            // 
            // UploadButton
            // 
            this.UploadButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.UploadButton.Location = new System.Drawing.Point(107, 35);
            this.UploadButton.Name = "UploadButton";
            this.UploadButton.Size = new System.Drawing.Size(75, 23);
            this.UploadButton.TabIndex = 2;
            this.UploadButton.Text = "Upload";
            this.UploadButton.UseVisualStyleBackColor = true;
            this.UploadButton.Click += new System.EventHandler(this.UploadButton_Click);
            // 
            // EvaluateButton
            // 
            this.EvaluateButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.EvaluateButton.Location = new System.Drawing.Point(107, 6);
            this.EvaluateButton.Name = "EvaluateButton";
            this.EvaluateButton.Size = new System.Drawing.Size(75, 23);
            this.EvaluateButton.TabIndex = 2;
            this.EvaluateButton.Text = "Evaluate";
            this.EvaluateButton.UseVisualStyleBackColor = true;
            this.EvaluateButton.Click += new System.EventHandler(this.EvaluateButton_Click);
            // 
            // UploadPathTxtBox
            // 
            this.UploadPathTxtBox.Location = new System.Drawing.Point(46, 37);
            this.UploadPathTxtBox.Name = "UploadPathTxtBox";
            this.UploadPathTxtBox.Size = new System.Drawing.Size(55, 20);
            this.UploadPathTxtBox.TabIndex = 1;
            this.UploadPathTxtBox.Text = "%EncSrc";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(37, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Shift";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(4, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(31, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Path";
            // 
            // ShiftTxtBox
            // 
            this.ShiftTxtBox.Location = new System.Drawing.Point(46, 8);
            this.ShiftTxtBox.Name = "ShiftTxtBox";
            this.ShiftTxtBox.Size = new System.Drawing.Size(55, 20);
            this.ShiftTxtBox.TabIndex = 0;
            this.ShiftTxtBox.Text = "1";
            // 
            // EncriptedRichTxtBox
            // 
            this.EncriptedRichTxtBox.AcceptsTab = true;
            this.EncriptedRichTxtBox.AutoWordSelection = true;
            this.EncriptedRichTxtBox.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.EncriptedRichTxtBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.EncriptedRichTxtBox.ForeColor = System.Drawing.SystemColors.Control;
            this.EncriptedRichTxtBox.Location = new System.Drawing.Point(3, 16);
            this.EncriptedRichTxtBox.Name = "EncriptedRichTxtBox";
            this.EncriptedRichTxtBox.ShowSelectionMargin = true;
            this.EncriptedRichTxtBox.Size = new System.Drawing.Size(517, 188);
            this.EncriptedRichTxtBox.TabIndex = 2;
            this.EncriptedRichTxtBox.Text = "";
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.VigenereToolBoxPanel);
            this.groupBox2.Controls.Add(this.DecriptedRichTxtBox);
            this.groupBox2.Controls.Add(this.EncriptedRichTxtBox);
            this.groupBox2.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox2.ForeColor = System.Drawing.SystemColors.Control;
            this.groupBox2.Location = new System.Drawing.Point(224, 13);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(526, 476);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Preview";
            // 
            // VigenereToolBoxPanel
            // 
            this.VigenereToolBoxPanel.ColumnCount = 3;
            this.VigenereToolBoxPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 15.5642F));
            this.VigenereToolBoxPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 69.06615F));
            this.VigenereToolBoxPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 15.5642F));
            this.VigenereToolBoxPanel.Controls.Add(this.VigenereEncryptButton, 0, 0);
            this.VigenereToolBoxPanel.Controls.Add(this.AnalyzeAlphasButton, 2, 0);
            this.VigenereToolBoxPanel.Controls.Add(this.VigenereKeyTxtBox, 1, 0);
            this.VigenereToolBoxPanel.Location = new System.Drawing.Point(6, 210);
            this.VigenereToolBoxPanel.Name = "VigenereToolBoxPanel";
            this.VigenereToolBoxPanel.RowCount = 1;
            this.VigenereToolBoxPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.VigenereToolBoxPanel.Size = new System.Drawing.Size(514, 29);
            this.VigenereToolBoxPanel.TabIndex = 5;
            // 
            // VigenereEncryptButton
            // 
            this.VigenereEncryptButton.Dock = System.Windows.Forms.DockStyle.Fill;
            this.VigenereEncryptButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.VigenereEncryptButton.Location = new System.Drawing.Point(3, 3);
            this.VigenereEncryptButton.Name = "VigenereEncryptButton";
            this.VigenereEncryptButton.Size = new System.Drawing.Size(73, 23);
            this.VigenereEncryptButton.TabIndex = 4;
            this.VigenereEncryptButton.Text = "Encrypt";
            this.VigenereEncryptButton.UseVisualStyleBackColor = true;
            this.VigenereEncryptButton.Click += new System.EventHandler(this.VigenereEncryptButton_Click);
            // 
            // AnalyzeAlphasButton
            // 
            this.AnalyzeAlphasButton.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.AnalyzeAlphasButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.AnalyzeAlphasButton.Location = new System.Drawing.Point(436, 3);
            this.AnalyzeAlphasButton.Name = "AnalyzeAlphasButton";
            this.AnalyzeAlphasButton.Size = new System.Drawing.Size(75, 23);
            this.AnalyzeAlphasButton.TabIndex = 4;
            this.AnalyzeAlphasButton.Text = "Analyze";
            this.AnalyzeAlphasButton.UseVisualStyleBackColor = true;
            this.AnalyzeAlphasButton.Click += new System.EventHandler(this.AnalyzeAlphasButton_Click);
            // 
            // VigenereKeyTxtBox
            // 
            this.VigenereKeyTxtBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.VigenereKeyTxtBox.Location = new System.Drawing.Point(82, 3);
            this.VigenereKeyTxtBox.Name = "VigenereKeyTxtBox";
            this.VigenereKeyTxtBox.Size = new System.Drawing.Size(348, 20);
            this.VigenereKeyTxtBox.TabIndex = 3;
            this.VigenereKeyTxtBox.Text = "VLADYSLAVSERHIIENKO";
            this.VigenereKeyTxtBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // DecriptedRichTxtBox
            // 
            this.DecriptedRichTxtBox.AcceptsTab = true;
            this.DecriptedRichTxtBox.AutoWordSelection = true;
            this.DecriptedRichTxtBox.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.DecriptedRichTxtBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.DecriptedRichTxtBox.ForeColor = System.Drawing.SystemColors.Control;
            this.DecriptedRichTxtBox.Location = new System.Drawing.Point(3, 245);
            this.DecriptedRichTxtBox.Name = "DecriptedRichTxtBox";
            this.DecriptedRichTxtBox.ShowSelectionMargin = true;
            this.DecriptedRichTxtBox.Size = new System.Drawing.Size(517, 225);
            this.DecriptedRichTxtBox.TabIndex = 2;
            this.DecriptedRichTxtBox.Text = "";
            // 
            // CaeserSandboxForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.ClientSize = new System.Drawing.Size(762, 501);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CaeserSandboxForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Caeser Sandbox";
            this.Activated += new System.EventHandler(this.CaeserSandboxForm_Activated);
            this.Deactivate += new System.EventHandler(this.CaeserSandboxForm_Deactivate);
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.CaeserSandboxForm_KeyPress);
            this.groupBox1.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.AlphaAnalysisGridView)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.VigenereToolBoxPanel.ResumeLayout(false);
            this.VigenereToolBoxPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox UploadPathTxtBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button EvaluateButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox ShiftTxtBox;
        private System.Windows.Forms.RichTextBox EncriptedRichTxtBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button UpdateButton;
        private System.Windows.Forms.TextBox DecSrcTxtBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox EncSrcTxtBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button UploadButton;
        private System.Windows.Forms.DataGridView AlphaAnalysisGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn EncColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn DecColumn;
        private System.Windows.Forms.Button ApplyButton;
        private System.Windows.Forms.RichTextBox DecriptedRichTxtBox;
        private System.Windows.Forms.Button HighlightButton;
        private System.Windows.Forms.TableLayoutPanel VigenereToolBoxPanel;
        private System.Windows.Forms.Button VigenereEncryptButton;
        private System.Windows.Forms.Button AnalyzeAlphasButton;
        private System.Windows.Forms.TextBox VigenereKeyTxtBox;
    }
}