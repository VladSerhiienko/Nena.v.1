using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;

namespace SmallTalk.Labwork
{
    public partial class WorkloadForm : Form
    {
        public WorkloadForm()
        {
            InitializeComponent();
        }

        public static void InspectChart()
        {
            var activeChart = Globals.ThisAddIn.Application.ActiveChart;
            if (activeChart != null) System.Diagnostics.Debug.WriteLine(
                " Style=" + ((int)activeChart.ChartStyle).ToString() +
                " Type=" + activeChart.ChartType.ToString()
                );
        }

        private void OnRunButtonClicked(object sender, EventArgs e)
        {

            var grubbsTask = new Laborarbeit.OutliersExclusionTask();
            var quantilesTask = new Laborarbeit.QuantilesLookupTask();
            var regrTask = new Laborarbeit.RegressionTask();
            var normTask = new Laborarbeit.NormalizationTask();
            var importTask = new Laborarbeit.ImportTask();
            var anovaTask = new Laborarbeit.AnovaTask();
            var lab = new Laborarbeit();

            importTask.SrcFile = "E:\\TESTS\\Tasks.xlsx";
            importTask.SrcTasksSheet = "KN44";
            importTask.SrcX1Sheet = "X1";
            importTask.SrcX2Sheet = "X2";
            importTask.SrcX3Sheet = "X3";
            importTask.SrcX4Sheet = "X4";
            importTask.SrcYSheet = "Y";
            importTask.DstSheet = "IMP";
            importTask.SrcTask = 14;
            importTask.SrcSize = 50;

            lab.ImportResults = importTask.Exec();
            lab.QuantilesLookup = quantilesTask.Exec();

            grubbsTask.ImportResults = lab.ImportResults;
            grubbsTask.Lookup = lab.QuantilesLookup;
            grubbsTask.DstSheet = "GT'A";

            var grubbResults = grubbsTask.Exec();

            normTask.SrcSample = grubbResults.DstSample;
            normTask.Parameter = -1;
            normTask.DesiredClassesCount = -1;
            normTask.DstFreqAnalysis = "FQ'A";
            normTask.DstChi2Analysis = "C2'A";
            normTask.DstNormPrefix = "N'";
            normTask.QuantilesLookup = lab.QuantilesLookup;

            var normResults = normTask.Exec();

            regrTask.SrcSample = grubbResults.DstSample;
            regrTask.QLookup = lab.QuantilesLookup;
            regrTask.RPrefix = "R'LQ";
            regrTask.Parameter = -1;

            var regrResults = regrTask.Exec();
            //anovaTask.SrcSample = grubbResults.DstSample;
            //anovaTask.FLookup = lab.QuantilesLookup;
            //var anovaResults = anovaTask.Exec();

            //MessageBox.Show("Done.");
        }

        private void OnCancelButtonClicked(object sender, EventArgs e)
        {
            InspectChart();

            this.Dispose();
            this.Close();
        }

        private void OnHelpButtonClicked(object sender, CancelEventArgs e)
        {
            /*MessageBox.Show(
                "Now you are up to define the labwork task you want my application to complete.\n" +
                "The Key/Value table you can see below indicates all the parameters needed.\n" +
                "Please, use it to change the default value to desired.\n" +
                "Many thanks and regards, Vlad\n" +
                "vlad.serhiienko@gmail.com"
                );*/
        }
    }
}
