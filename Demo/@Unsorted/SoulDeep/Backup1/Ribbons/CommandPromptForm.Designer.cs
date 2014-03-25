namespace SmallTalk.Ribbons
{
    partial class CommandPromptForm
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
            this.CommandListenerBox = new System.Windows.Forms.GroupBox();
            this.CommandListBox = new System.Windows.Forms.ListBox();
            this.CommandTextPanel = new System.Windows.Forms.Panel();
            this.RunFromSelectionButton = new System.Windows.Forms.Button();
            this.RunButton = new System.Windows.Forms.Button();
            this.CommandTxtBox = new System.Windows.Forms.TextBox();
            this.CommandListenerBox.SuspendLayout();
            this.CommandTextPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // CommandListenerBox
            // 
            this.CommandListenerBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.CommandListenerBox.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CommandListenerBox.Controls.Add(this.CommandListBox);
            this.CommandListenerBox.Controls.Add(this.CommandTextPanel);
            this.CommandListenerBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.CommandListenerBox.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.CommandListenerBox.ForeColor = System.Drawing.SystemColors.Control;
            this.CommandListenerBox.Location = new System.Drawing.Point(12, 12);
            this.CommandListenerBox.Name = "CommandListenerBox";
            this.CommandListenerBox.Size = new System.Drawing.Size(660, 437);
            this.CommandListenerBox.TabIndex = 0;
            this.CommandListenerBox.TabStop = false;
            this.CommandListenerBox.Text = "Command listener";
            // 
            // CommandListBox
            // 
            this.CommandListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.CommandListBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.CommandListBox.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.CommandListBox.FormattingEnabled = true;
            this.CommandListBox.Location = new System.Drawing.Point(7, 59);
            this.CommandListBox.Name = "CommandListBox";
            this.CommandListBox.Size = new System.Drawing.Size(646, 366);
            this.CommandListBox.TabIndex = 1;
            // 
            // CommandTextPanel
            // 
            this.CommandTextPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.CommandTextPanel.Controls.Add(this.RunFromSelectionButton);
            this.CommandTextPanel.Controls.Add(this.RunButton);
            this.CommandTextPanel.Controls.Add(this.CommandTxtBox);
            this.CommandTextPanel.Location = new System.Drawing.Point(7, 20);
            this.CommandTextPanel.Name = "CommandTextPanel";
            this.CommandTextPanel.Size = new System.Drawing.Size(646, 50);
            this.CommandTextPanel.TabIndex = 0;
            // 
            // RunFromSelectionButton
            // 
            this.RunFromSelectionButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.RunFromSelectionButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.RunFromSelectionButton.Location = new System.Drawing.Point(533, 4);
            this.RunFromSelectionButton.Name = "RunFromSelectionButton";
            this.RunFromSelectionButton.Size = new System.Drawing.Size(109, 23);
            this.RunFromSelectionButton.TabIndex = 2;
            this.RunFromSelectionButton.Text = "From selection";
            this.RunFromSelectionButton.UseVisualStyleBackColor = true;
            this.RunFromSelectionButton.Click += new System.EventHandler(this.RunFromSelectionButton_Click);
            // 
            // RunButton
            // 
            this.RunButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.RunButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.RunButton.Location = new System.Drawing.Point(470, 4);
            this.RunButton.Name = "RunButton";
            this.RunButton.Size = new System.Drawing.Size(57, 23);
            this.RunButton.TabIndex = 1;
            this.RunButton.Text = "Run";
            this.RunButton.UseVisualStyleBackColor = true;
            this.RunButton.Click += new System.EventHandler(this.RunButton_Click);
            // 
            // CommandTxtBox
            // 
            this.CommandTxtBox.AllowDrop = true;
            this.CommandTxtBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.CommandTxtBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.CommandTxtBox.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.CommandTxtBox.Location = new System.Drawing.Point(4, 6);
            this.CommandTxtBox.Multiline = true;
            this.CommandTxtBox.Name = "CommandTxtBox";
            this.CommandTxtBox.Size = new System.Drawing.Size(460, 20);
            this.CommandTxtBox.TabIndex = 0;
            this.CommandTxtBox.Text = "$ ";
            this.CommandTxtBox.DragDrop += new System.Windows.Forms.DragEventHandler(this.CommandTxtBox_DragDrop);
            this.CommandTxtBox.DragEnter += new System.Windows.Forms.DragEventHandler(this.CommandTxtBox_DragEnter);
            this.CommandTxtBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.CommandTxtBox_KeyPress);
            // 
            // CommandPromptForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.ClientSize = new System.Drawing.Size(684, 461);
            this.Controls.Add(this.CommandListenerBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Location = new System.Drawing.Point(500, 200);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CommandPromptForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Flow control";
            this.Activated += new System.EventHandler(this.CommandPromptForm_Activated);
            this.Deactivate += new System.EventHandler(this.CommandPromptForm_Deactivate);
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.CommandPromptForm_KeyPress);
            this.CommandListenerBox.ResumeLayout(false);
            this.CommandTextPanel.ResumeLayout(false);
            this.CommandTextPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox CommandListenerBox;
        private System.Windows.Forms.Panel CommandTextPanel;
        private System.Windows.Forms.Button RunFromSelectionButton;
        private System.Windows.Forms.Button RunButton;
        private System.Windows.Forms.TextBox CommandTxtBox;
        public System.Windows.Forms.ListBox CommandListBox;
    }
}