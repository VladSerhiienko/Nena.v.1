namespace SmallTalk
{
    partial class Statistics : Microsoft.Office.Tools.Ribbon.RibbonBase
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        public Statistics()
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
            this.StatisticsRbbnTab = this.Factory.CreateRibbonTab();
            this.LabworksTab = this.Factory.CreateRibbonGroup();
            this.LaunchBttn = this.Factory.CreateRibbonButton();
            this.StatisticsRbbnTab.SuspendLayout();
            this.LabworksTab.SuspendLayout();
            // 
            // StatisticsRbbnTab
            // 
            this.StatisticsRbbnTab.ControlId.ControlIdType = Microsoft.Office.Tools.Ribbon.RibbonControlIdType.Office;
            this.StatisticsRbbnTab.Groups.Add(this.LabworksTab);
            this.StatisticsRbbnTab.Label = "SmallTalk";
            this.StatisticsRbbnTab.Name = "StatisticsRbbnTab";
            // 
            // LabworksTab
            // 
            this.LabworksTab.Items.Add(this.LaunchBttn);
            this.LabworksTab.Label = "Labworks";
            this.LabworksTab.Name = "LabworksTab";
            // 
            // LaunchBttn
            // 
            this.LaunchBttn.Label = "Launch";
            this.LaunchBttn.Name = "LaunchBttn";
            this.LaunchBttn.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.OnLaunchButtonClicked);
            // 
            // Statistics
            // 
            this.Name = "Statistics";
            this.RibbonType = "Microsoft.Excel.Workbook";
            this.Tabs.Add(this.StatisticsRbbnTab);
            this.Load += new Microsoft.Office.Tools.Ribbon.RibbonUIEventHandler(this.Statistics_Load);
            this.StatisticsRbbnTab.ResumeLayout(false);
            this.StatisticsRbbnTab.PerformLayout();
            this.LabworksTab.ResumeLayout(false);
            this.LabworksTab.PerformLayout();

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab StatisticsRbbnTab;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup LabworksTab;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton LaunchBttn;
    }

    partial class ThisRibbonCollection
    {
        internal Statistics Statistics
        {
            get { return this.GetRibbon<Statistics>(); }
        }
    }
}
