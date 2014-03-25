using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Office.Tools.Ribbon;

using SmallTalk.FlowControl;
using SmallTalk.Ribbons;

namespace SmallTalk
{
    public partial class HauptMenu
    {
        private void HauptMenu_Load(object sender, RibbonUIEventArgs e)
        {

        }

        private void CommandToolButton_Click(object sender, RibbonControlEventArgs e)
        {
            var cmdForm = new CommandPromptForm();
            cmdForm.Show();
        }

        private void CaeserSandboxButton_Click(object sender, RibbonControlEventArgs e)
        {
            var caeserBox = new CaeserSandboxForm();
            caeserBox.Show();
        }

        private void CryptoSandboxButton_Click(object sender, RibbonControlEventArgs e)
        {
            var cryptoBox = new CryptoSandbox();
            cryptoBox.Show();
        }
    }
}
