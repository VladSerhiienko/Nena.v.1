using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InteractiveTV.InlineServer
{
    public enum DeviceStatus
    {
        Disconnected,
        Reconnected,
    }

    public enum Status
    {
        DeviceDisconnected,
        DeviceReconnected,
        Disposed,
        Critical,
        Streaming,
        Success,
        Failure,
    }

    public interface IGestureServiceInline
    {
        string Device { get; set; }
        string Module { get; set; }
        void Launch();
        void Stop();
    }
}
