using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace InteractiveTV.InlineServer
{
    public class GestureServiceInline : IGestureServiceInline
    {
        public delegate void UpdateEventHandler(IGestureServiceInline sender, UpdateEvent e);
        public delegate void StatusChangedEventHandler(IGestureServiceInline sender, StatusChangedEvent e);
        public delegate void DeviceConnectionChangedHandler(IGestureServiceInline sender, DeviceStatus connected);

        private bool disconnected;
        private PXCMGesture.GeoNode[][] nodes;
        private PXCMGesture.Gesture[] gestures;

        private Task procRoutine;
        private CancellationTokenSource procTokenSource;

        public string Device { get; set; }
        public string Module { get; set; }
        public event UpdateEventHandler Updated;
        public event StatusChangedEventHandler StatusChanged;
        public event DeviceConnectionChangedHandler DeviceConnectedChanged;

        public GestureServiceInline()
        {
            Device = string.Empty;
            Module = string.Empty;
            disconnected = false;
        }

        private bool IsSubscribedFor(
            IConsumeGestureService consumer, 
            Requirement requirements
            )
        {
            return (consumer.Requirements & requirements) == requirements;
        }

        private void Update(PXCMGesture gesture)
        {
            gesture.QueryNodeData(0, PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_PRIMARY, nodes[0]);
            gesture.QueryNodeData(0, PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_SECONDARY, nodes[1]);
            gesture.QueryNodeData(0, PXCMGesture.GeoNode.Label.LABEL_BODY_ELBOW_PRIMARY, out nodes[0][nodes.Length - 1]);
            gesture.QueryNodeData(0, PXCMGesture.GeoNode.Label.LABEL_BODY_ELBOW_SECONDARY, out nodes[1][nodes.Length - 1]);
            gesture.QueryGestureData(0, PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_PRIMARY, 0, out gestures[0]);
            gesture.QueryGestureData(0, PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_SECONDARY, 0, out gestures[1]);
            Updated(this, new UpdateEvent() { Geonodes = nodes, Gestures = gestures });
        }

        private bool CheckDeviceConnection(bool state)
        {
            if (state)
            {
                if (!disconnected)
                {
                    StatusChanged(this, new StatusChangedEvent
                    {
                        Token = Status.Failure,
                        Description = "Device disconnected"
                    });

                    DeviceConnectedChanged(this, DeviceStatus.Disconnected);
                }

                disconnected = true;
            }
            else
            {
                if (disconnected)
                {
                    StatusChanged(this, new StatusChangedEvent
                    {
                        Token = Status.Success,
                        Description = "Device reconnected"
                    });

                    DeviceConnectedChanged(this, DeviceStatus.Reconnected);
                }

                disconnected = false;
            }

            return disconnected;
        }

        private void ServiceRoutine(CancellationToken token)
        {
            gestures = new PXCMGesture.Gesture[2];
            nodes = new PXCMGesture.GeoNode[2][] 
            { 
                new PXCMGesture.GeoNode[11], 
                new PXCMGesture.GeoNode[11] 
            };

            PXCMSession session;
            pxcmStatus sts = PXCMSession.CreateInstance(out session);

            if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR)
            {
                StatusChanged( this, new StatusChangedEvent 
                {
                        Token = Status.Critical,
                        Description = "Failed to create an SDK session" 
                }); return;
            }

            PXCMSession.ImplDesc desc = new PXCMSession.ImplDesc();
            desc.friendlyName.set(Module);

            PXCMGesture gesture;
            sts = session.CreateImpl<PXCMGesture>(ref desc, PXCMGesture.CUID, out gesture);
            if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR)
            {
                StatusChanged(this, new StatusChangedEvent
                {
                    Token = Status.Critical,
                    Description = "Failed to create the gesture module"
                });
                session.Dispose();
                return;
            }

            UtilMCapture capture = null;
            capture = new UtilMCapture(session);
            capture.SetFilter(Device);

            StatusChanged(this, new StatusChangedEvent
            {
                Token = Status.Success,
                Description = "Pair moudle with I/O"
            });

            for (uint i = 0; ; i++)
            {
                PXCMGesture.ProfileInfo pinfo;
                sts = gesture.QueryProfile(i, out pinfo);
                if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR) break;
                sts = capture.LocateStreams(ref pinfo.inputs);
                if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR) continue;
                sts = gesture.SetProfile(ref pinfo);
                if (sts >= pxcmStatus.PXCM_STATUS_NO_ERROR) break;
            }
            if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR)
            {
                StatusChanged(this, new StatusChangedEvent
                {
                    Token = Status.Disposed |
                            Status.Critical,
                    Description = "Failed to pair the gesture module with I/O"
                });

                capture.Dispose();
                gesture.Dispose();
                session.Dispose();

                return;
            }

            StatusChanged(this, new StatusChangedEvent
            {
                Token = Status.Streaming,
                Description = "Initialization succeeded"
            });

            PXCMImage[] images = new PXCMImage[PXCMCapture.VideoStream.STREAM_LIMIT];
            PXCMScheduler.SyncPoint[] sps = new PXCMScheduler.SyncPoint[2];

            while (!token.IsCancellationRequested)
            {
                PXCMImage.Dispose(images);
                PXCMScheduler.SyncPoint.Dispose(sps);

                sts = capture.ReadStreamAsync(images, out sps[0]);
                if (CheckDeviceConnection(sts == pxcmStatus.PXCM_STATUS_DEVICE_LOST)) continue;
                if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR) break;

                sts = gesture.ProcessImageAsync(images, out sps[1]);
                if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR) break;
                PXCMScheduler.SyncPoint.SynchronizeEx(sps);

                sts = sps[0].Synchronize();
                if (CheckDeviceConnection(sts == pxcmStatus.PXCM_STATUS_DEVICE_LOST)) continue;
                if (sts < pxcmStatus.PXCM_STATUS_NO_ERROR) break;

                // PXCMImage depth = capture.QueryImage(images, PXCMImage.ImageType.IMAGE_TYPE_DEPTH);
                // DisplayPicture(depth, gesture);

                Update(gesture);

                //form.UpdatePanel();
            }

            PXCMImage.Dispose(images);
            PXCMScheduler.SyncPoint.Dispose(sps);

            capture.Dispose();
            gesture.Dispose();
            session.Dispose();

            StatusChanged(this, new StatusChangedEvent
            {
                Token = Status.Success | 
                        Status.Disposed,
                Description = "Exiting service routine"
            });

        }

        public void Launch()
        {
            lock (this)
            {
                if (procTokenSource != null) return;
                procTokenSource = new CancellationTokenSource();
                procRoutine = Task.Run(
                    () => { ServiceRoutine(procTokenSource.Token); },
                    procTokenSource.Token
                    );
            }
        }

        public void Stop()
        {
            try
            {
                procTokenSource.Cancel();
                procRoutine.Wait();
                if (procTokenSource != null) procTokenSource.Dispose();
                if (procRoutine != null) procRoutine.Dispose();
            }
            catch 
            {
                StatusChanged(this, new StatusChangedEvent
                {
                    Token = Status.Failure | Status.Disposed,
                    Description = "Errors occured when stopping gesture service"
                });
            }
        }

    }
}
