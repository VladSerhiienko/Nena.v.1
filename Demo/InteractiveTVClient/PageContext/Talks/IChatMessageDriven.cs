using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InteractiveTV.PageContext.Talks
{
    public interface IChatMessageDriven
    {
        void OnReceivedMessage(object sender, AppTalk.ChatEvent e);
        void OnSendingMessage(object sender, AppTalk.ChatEvent e);
    }
}
