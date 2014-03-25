#include "VideoCapturePerc.h"

#ifndef __NENA_VIDEOFACEPERC_INCLUDED__
#define __NENA_VIDEOFACEPERC_INCLUDED__

namespace Nena
{
	namespace Video
	{
		namespace Perc
		{
			namespace Tracking
			{
				struct Face : public Utility::BasicStatus<>
				{
					static UINT32 const DefaultProfile = UINT_ERROR;
					static UINT32 const FirstSuccessfulQuery = UINT_ERROR - 1;

					typedef void Action;
					typedef HRESULT Result;
					typedef pxcStatus Status;

					PXCSmartPtr<PXCFaceAnalysis> Base;

					PXCFaceAnalysis::Landmark *Landmark;
					PXCFaceAnalysis::Detection *Detection;

					BOOL Paused;
					BOOL Enabled;
					UINT32 DescIndex;
					PXCSession::ImplDesc Desc;
					PXCFaceAnalysis::ProfileInfo Profile;
					PXCFaceAnalysis::Landmark::ProfileInfo LandmarkProfile;
					PXCFaceAnalysis::Detection::ProfileInfo DetectionProfile;

					Face(_In_ Capture *capture);
					Face(_In_ PXCSession *session);

					Action Init(_In_ Capture *capture);
					Action Init(_In_ PXCSession *session);
					Action PauseFaceAnalysis(_In_ BOOL paused);
					Action SubmitRequirements(_Inout_ Capture *host);

					Action SetProfile(void);
					Action SetLandmarkProfile(void);
					Action SetDetectionProfile(void);
					Action SearchProfile(_In_opt_ UINT32 option = FirstSuccessfulQuery);
					Action SearchLandmarkProfile(_In_opt_ UINT32 option = FirstSuccessfulQuery);
					Action SearchDetectionProfile(_In_opt_ UINT32 option = FirstSuccessfulQuery);

					Status SubmitProcessTask(_Inout_ Sample *sample);
					Status SubmitProcessTask(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync);
					Status SubmitProcessTask(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);

					Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample);
					Status SubmitProcessTaskUnsafe(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync);
					Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);

					UINT32 GetLastFaceCount(_Inout_opt_ Status *lastStatus = nullptr);

				};
			}
		}
	}
}

#endif // !__NENA_VIDEOFACEPERC_INCLUDED__
