using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using InteractiveTV.InlineServer;

namespace InteractiveTV.PageContext.Talks
{
    class GestureConsumerInlined : IConsumeGestureService
    {
        public Requirement Requirements
        {
            get { throw new NotImplementedException(); }
        }

        public void OnUpdated(IGestureServiceInline sender, UpdateEvent e)
        {
        }

        public void OnStatusChanged(IGestureServiceInline sender, StatusChangedEvent e)
        {
        }
    }
}
