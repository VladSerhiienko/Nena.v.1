#include "VideoCapturePerc.h"

#ifndef __NENA_VIDEOGESTUREPERC_INCLUDED__
#define __NENA_VIDEOGESTUREPERC_INCLUDED__

namespace Nena
{
	namespace Video
	{
		namespace Perc
		{
			namespace Tracking
			{
				struct Gesture : public Utility::BasicStatus<>
				{
					static UINT32 const DefaultProfile = UINT_ERROR;
					static UINT32 const FirstSuccessfulQuery = UINT_ERROR - 1;

					typedef void Action;
					typedef HRESULT Result;
					typedef pxcStatus Status;

					typedef struct DebugOutput :
						protected PXCGesture::Gesture::Handler,
						protected PXCGesture::Alert::Handler
					{
						typedef PXCGesture::Gesture::Handler *HGesture;
						typedef PXCGesture::Alert::Handler *HAlert;

						inline HAlert GetAlertHandler() { return this; }
						inline HGesture GetGestureHandler() { return this; }
						virtual Action PXCAPI OnAlert(_In_ PXCGesture::Alert *) override;
						virtual Action PXCAPI OnGesture(_In_ PXCGesture::Gesture *) override;
					};

					BOOL Paused;
					BOOL Enabled;
					UINT32 DescIndex;
					PXCSession::ImplDesc Desc;
					PXCGesture::ProfileInfo Profile;
					PXCSmartPtr<PXCGesture> Base;

					Gesture(_In_ Capture *capture);
					Gesture(_In_ PXCSession *session);

					Action Init(_In_ Capture *capture);
					Action Init(_In_ PXCSession *session);
					Action PauseGesture(_In_ BOOL paused);
					Action EnableGesture(_In_ pxcUID iuid = 0);
					Action EnableGesture(_In_ pxcCHAR *name);
					Action SubmitRequirements(_Inout_ Capture *host);
					Action SetProfile(void);
					Action SearchProfile(_In_opt_ UINT32 option = DefaultProfile);
					Status Subscribe(_In_ PXCGesture::Gesture::Handler *gesture = nullptr);
					Status Subscribe(_In_ PXCGesture::Alert::Handler *alert = nullptr);
					Status SubmitProcessTask(_Inout_ Sample *sample);
					Status SubmitProcessTask(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync);
					Status SubmitProcessTask(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);
					Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample);
					Status SubmitProcessTaskUnsafe(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync);
					Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);

				};
			}
		}
	}
}

#endif // !__NENA_VIDEOGESTUREPERC_INCLUDED__
