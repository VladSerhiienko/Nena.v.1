namespace SmallTalk
{
    partial class HauptMenu : Microsoft.Office.Tools.Ribbon.RibbonBase
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        public HauptMenu()
            : base(Globals.Factory.GetRibbonFactory())
        {
            InitializeComponent();
        }

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

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SoulDeepTab = this.Factory.CreateRibbonTab();
            this.FlowGroup = this.Factory.CreateRibbonGroup();
            this.CommandToolButton = this.Factory.CreateRibbonButton();
            this.CryptoGroup = this.Factory.CreateRibbonGroup();
            this.CryptoSandboxButton = this.Factory.CreateRibbonButton();
            this.CaeserToolGroup = this.Factory.CreateRibbonGroup();
            this.CaeserSandboxButton = this.Factory.CreateRibbonButton();
            this.SoulDeepTab.SuspendLayout();
            this.FlowGroup.SuspendLayout();
            this.CryptoGroup.SuspendLayout();
            this.CaeserToolGroup.SuspendLayout();
            // 
            // SoulDeepTab
            // 
            this.SoulDeepTab.ControlId.ControlIdType = Microsoft.Office.Tools.Ribbon.RibbonControlIdType.Office;
            this.SoulDeepTab.Groups.Add(this.FlowGroup);
            this.SoulDeepTab.Groups.Add(this.CryptoGroup);
            this.SoulDeepTab.Groups.Add(this.CaeserToolGroup);
            this.SoulDeepTab.Label = "Soul Deep";
            this.SoulDeepTab.Name = "SoulDeepTab";
            // 
            // FlowGroup
            // 
            this.FlowGroup.Items.Add(this.CommandToolButton);
            this.FlowGroup.Label = "Work Flow";
            this.FlowGroup.Name = "FlowGroup";
            // 
            // CommandToolButton
            // 
            this.CommandToolButton.Label = "Command Tool";
            this.CommandToolButton.Name = "CommandToolButton";
            this.CommandToolButton.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.CommandToolButton_Click);
            // 
            // CryptoGroup
            // 
            this.CryptoGroup.Items.Add(this.CryptoSandboxButton);
            this.CryptoGroup.Label = "Crypto Tools";
            this.CryptoGroup.Name = "CryptoGroup";
            // 
            // CryptoSandboxButton
            // 
            this.CryptoSandboxButton.Label = "Sandbox";
            this.CryptoSandboxButton.Name = "CryptoSandboxButton";
            this.CryptoSandboxButton.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.CryptoSandboxButton_Click);
            // 
            // CaeserToolGroup
            // 
            this.CaeserToolGroup.Items.Add(this.CaeserSandboxButton);
            this.CaeserToolGroup.Label = "Caeser Cipher Tool";
            this.CaeserToolGroup.Name = "CaeserToolGroup";
            // 
            // CaeserSandboxButton
            // 
            this.CaeserSandboxButton.Enabled = false;
            this.CaeserSandboxButton.Label = "Sandbox";
            this.CaeserSandboxButton.Name = "CaeserSandboxButton";
            this.CaeserSandboxButton.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.CaeserSandboxButton_Click);
            // 
            // HauptMenu
            // 
            this.Name = "HauptMenu";
            this.RibbonType = "Microsoft.Excel.Workbook";
            this.Tabs.Add(this.SoulDeepTab);
            this.Load += new Microsoft.Office.Tools.Ribbon.RibbonUIEventHandler(this.HauptMenu_Load);
            this.SoulDeepTab.ResumeLayout(false);
            this.SoulDeepTab.PerformLayout();
            this.FlowGroup.ResumeLayout(false);
            this.FlowGroup.PerformLayout();
            this.CryptoGroup.ResumeLayout(false);
            this.CryptoGroup.PerformLayout();
            this.CaeserToolGroup.ResumeLayout(false);
            this.CaeserToolGroup.PerformLayout();

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab SoulDeepTab;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup FlowGroup;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton CommandToolButton;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup CaeserToolGroup;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton CaeserSandboxButton;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup CryptoGroup;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton CryptoSandboxButton;
    }

    partial class ThisRibbonCollection
    {
        internal HauptMenu HauptMenu
        {
            get { return this.GetRibbon<HauptMenu>(); }
        }
    }
}
