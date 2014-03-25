namespace SmallTalk.Ribbons
{
    partial class CryptoSandbox
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.KeyValuePairGridView = new System.Windows.Forms.DataGridView();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.KeyClmn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ValueClmn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.DecRichTextBox = new System.Windows.Forms.RichTextBox();
            this.splitContainer4 = new System.Windows.Forms.SplitContainer();
            this.EncRichTextBox = new System.Windows.Forms.RichTextBox();
            this.AnalyzeDecButton = new System.Windows.Forms.Button();
            this.AnalyzeEncButton = new System.Windows.Forms.Button();
            this.DecEncButton = new System.Windows.Forms.Button();
            this.EncDecButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.KeyValuePairGridView)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).BeginInit();
            this.splitContainer4.Panel1.SuspendLayout();
            this.splitContainer4.Panel2.SuspendLayout();
            this.splitContainer4.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.groupBox2);
            this.splitContainer1.Size = new System.Drawing.Size(867, 469);
            this.splitContainer1.SplitterDistance = 208;
            this.splitContainer1.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.KeyValuePairGridView);
            this.groupBox1.Location = new System.Drawing.Point(4, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 462);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Toolbox";
            // 
            // KeyValuePairGridView
            // 
            this.KeyValuePairGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.KeyValuePairGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.KeyClmn,
            this.ValueClmn});
            this.KeyValuePairGridView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KeyValuePairGridView.Location = new System.Drawing.Point(3, 16);
            this.KeyValuePairGridView.Name = "KeyValuePairGridView";
            this.KeyValuePairGridView.Size = new System.Drawing.Size(194, 443);
            this.KeyValuePairGridView.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.splitContainer2);
            this.groupBox2.Location = new System.Drawing.Point(4, 4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(648, 462);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Preview";
            // 
            // KeyClmn
            // 
            this.KeyClmn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCells;
            this.KeyClmn.HeaderText = "Key";
            this.KeyClmn.Name = "KeyClmn";
            this.KeyClmn.Width = 50;
            // 
            // ValueClmn
            // 
            this.ValueClmn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.ValueClmn.HeaderText = "Value";
            this.ValueClmn.Name = "ValueClmn";
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(3, 16);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.splitContainer3);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer4);
            this.splitContainer2.Size = new System.Drawing.Size(642, 443);
            this.splitContainer2.SplitterDistance = 214;
            this.splitContainer2.TabIndex = 0;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 0);
            this.splitContainer3.Name = "splitContainer3";
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.EncDecButton);
            this.splitContainer3.Panel1.Controls.Add(this.AnalyzeDecButton);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.DecRichTextBox);
            this.splitContainer3.Size = new System.Drawing.Size(642, 214);
            this.splitContainer3.SplitterDistance = 97;
            this.splitContainer3.TabIndex = 0;
            // 
            // DecRichTextBox
            // 
            this.DecRichTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.DecRichTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.DecRichTextBox.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.DecRichTextBox.Location = new System.Drawing.Point(0, 0);
            this.DecRichTextBox.Name = "DecRichTextBox";
            this.DecRichTextBox.Size = new System.Drawing.Size(541, 214);
            this.DecRichTextBox.TabIndex = 0;
            this.DecRichTextBox.Text = "<Decrypted text>";
            // 
            // splitContainer4
            // 
            this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer4.Location = new System.Drawing.Point(0, 0);
            this.splitContainer4.Name = "splitContainer4";
            // 
            // splitContainer4.Panel1
            // 
            this.splitContainer4.Panel1.Controls.Add(this.DecEncButton);
            this.splitContainer4.Panel1.Controls.Add(this.AnalyzeEncButton);
            // 
            // splitContainer4.Panel2
            // 
            this.splitContainer4.Panel2.Controls.Add(this.EncRichTextBox);
            this.splitContainer4.Size = new System.Drawing.Size(642, 225);
            this.splitContainer4.SplitterDistance = 97;
            this.splitContainer4.TabIndex = 0;
            // 
            // EncRichTextBox
            // 
            this.EncRichTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.EncRichTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.EncRichTextBox.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.EncRichTextBox.Location = new System.Drawing.Point(0, 0);
            this.EncRichTextBox.Name = "EncRichTextBox";
            this.EncRichTextBox.Size = new System.Drawing.Size(541, 225);
            this.EncRichTextBox.TabIndex = 0;
            this.EncRichTextBox.Text = "<Encrypted text>";
            // 
            // AnalyzeDecButton
            // 
            this.AnalyzeDecButton.Dock = System.Windows.Forms.DockStyle.Top;
            this.AnalyzeDecButton.Location = new System.Drawing.Point(0, 0);
            this.AnalyzeDecButton.Name = "AnalyzeDecButton";
            this.AnalyzeDecButton.Size = new System.Drawing.Size(97, 23);
            this.AnalyzeDecButton.TabIndex = 0;
            this.AnalyzeDecButton.Text = "Analyze";
            this.AnalyzeDecButton.UseVisualStyleBackColor = true;
            this.AnalyzeDecButton.Click += new System.EventHandler(this.AnalyzeDecButton_Click);
            // 
            // AnalyzeEncButton
            // 
            this.AnalyzeEncButton.Dock = System.Windows.Forms.DockStyle.Top;
            this.AnalyzeEncButton.Location = new System.Drawing.Point(0, 0);
            this.AnalyzeEncButton.Name = "AnalyzeEncButton";
            this.AnalyzeEncButton.Size = new System.Drawing.Size(97, 23);
            this.AnalyzeEncButton.TabIndex = 1;
            this.AnalyzeEncButton.Text = "Analyze";
            this.AnalyzeEncButton.UseVisualStyleBackColor = true;
            this.AnalyzeEncButton.Click += new System.EventHandler(this.AnalyzeEncButton_Click);
            // 
            // DecEncButton
            // 
            this.DecEncButton.Dock = System.Windows.Forms.DockStyle.Top;
            this.DecEncButton.Location = new System.Drawing.Point(0, 23);
            this.DecEncButton.Name = "DecEncButton";
            this.DecEncButton.Size = new System.Drawing.Size(97, 23);
            this.DecEncButton.TabIndex = 2;
            this.DecEncButton.Text = "Decrypt";
            this.DecEncButton.UseVisualStyleBackColor = true;
            this.DecEncButton.Click += new System.EventHandler(this.DecEncButton_Click);
            // 
            // EncDecButton
            // 
            this.EncDecButton.Dock = System.Windows.Forms.DockStyle.Top;
            this.EncDecButton.Location = new System.Drawing.Point(0, 23);
            this.EncDecButton.Name = "EncDecButton";
            this.EncDecButton.Size = new System.Drawing.Size(97, 23);
            this.EncDecButton.TabIndex = 1;
            this.EncDecButton.Text = "Encrypt";
            this.EncDecButton.UseVisualStyleBackColor = true;
            this.EncDecButton.Click += new System.EventHandler(this.EncDecButton_Click);
            // 
            // CryptoSandbox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(867, 469);
            this.Controls.Add(this.splitContainer1);
            this.Name = "CryptoSandbox";
            this.Text = "Sandbox";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.KeyValuePairGridView)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.splitContainer4.Panel1.ResumeLayout(false);
            this.splitContainer4.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).EndInit();
            this.splitContainer4.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.DataGridView KeyValuePairGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn KeyClmn;
        private System.Windows.Forms.DataGridViewTextBoxColumn ValueClmn;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.RichTextBox DecRichTextBox;
        private System.Windows.Forms.SplitContainer splitContainer4;
        private System.Windows.Forms.RichTextBox EncRichTextBox;
        private System.Windows.Forms.Button EncDecButton;
        private System.Windows.Forms.Button AnalyzeDecButton;
        private System.Windows.Forms.Button DecEncButton;
        private System.Windows.Forms.Button AnalyzeEncButton;
    }
}