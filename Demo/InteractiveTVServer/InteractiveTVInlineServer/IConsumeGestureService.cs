using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InteractiveTV.InlineServer
{
    public class UpdateEvent
    {
        public PXCMGesture.Gesture[] Gestures { get; set; }
        public PXCMGesture.GeoNode[][] Geonodes { get; set; }
    }

    public class StatusChangedEvent
    {
        public Status Token { get; set; }
        public System.String Description { get; set; }
    }

    public enum Requirement
    {
        Gestures,
        Geonodes,
    }

    public enum Label
    {
        SwipeRight = PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_RIGHT,
        SwipeLeft = PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_LEFT,
        SwipeDown = PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_DOWN,
        SwipeUp = PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_UP,
        ThumbDown = PXCMGesture.Gesture.Label.LABEL_POSE_THUMB_DOWN,
        ThumbUp = PXCMGesture.Gesture.Label.LABEL_POSE_THUMB_UP,
        Circle = PXCMGesture.Gesture.Label.LABEL_HAND_CIRCLE,
        Wave = PXCMGesture.Gesture.Label.LABEL_HAND_WAVE,
        Peace = PXCMGesture.Gesture.Label.LABEL_POSE_PEACE,
        Big5 = PXCMGesture.Gesture.Label.LABEL_POSE_BIG5,
    }

    public interface IConsumeGestureService
    {
        Requirement Requirements { get; }
        void OnUpdated(IGestureServiceInline sender, UpdateEvent e);
        void OnStatusChanged(IGestureServiceInline sender, StatusChangedEvent e);
    }
}
