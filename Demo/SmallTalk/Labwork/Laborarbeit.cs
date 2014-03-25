using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;

namespace SmallTalk.Labwork
{
    public partial class Laborarbeit
    {
        public static Air.Results Logger;
        public ImportTask.Results ImportResults;
        public QuantilesLookupTask.Results QuantilesLookup;

        public Laborarbeit(string air = "AIR")
        {
            if (Logger == null) Logger = (new Air() { LogName = air }).Exec();
            (Logger.Log.Columns[3] as Excel.Range).ColumnWidth = 12;
            (Logger.Log.Columns[4] as Excel.Range).ColumnWidth = 9;
        }
    }
}
