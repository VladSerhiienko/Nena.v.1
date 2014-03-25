using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SmallTalk.Ribbons
{
    class CommandListBox : ListBox
    {
        public CommandListBox() : base()
        {
        }

        private const int LB_ADDSTRING = 0x180;
        private const int LB_INSERTSTRING = 0x181;
        private const int LB_DELETESTRING = 0x182;
        private const int LB_RESETCONTENT = 0x184;

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == LB_ADDSTRING ||
                m.Msg == LB_INSERTSTRING ||
                m.Msg == LB_DELETESTRING ||
                m.Msg == LB_RESETCONTENT)
            {
                ItemsChanged();
            }

            base.WndProc(ref m);
        }



        private void ItemsChanged()
        {
            SelectedIndex = Items.Count - 1;
            SelectedIndex = -1;
        }
    }
}
