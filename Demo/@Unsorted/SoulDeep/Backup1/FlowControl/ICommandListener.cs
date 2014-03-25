using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;
using SmallTalk.Ribbons;

namespace SmallTalk.FlowControl
{
    interface ICommandListener
    {
        void Handle(CommandPromptForm sender, ICommand cmd, int start, int end);
    }


}
