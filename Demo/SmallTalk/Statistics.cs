using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Office.Tools.Ribbon;

using SmallTalk.Labwork;

namespace SmallTalk
{
    public partial class Statistics
    {
        private void Statistics_Load(object sender, RibbonUIEventArgs e)
        {

        }

        private void OnLaunchButtonClicked(object sender, RibbonControlEventArgs e)
        {
            var workload = new WorkloadForm();
            workload.Show();
        }
    }
}
