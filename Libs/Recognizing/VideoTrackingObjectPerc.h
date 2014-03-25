#include "VideoCapturePerc.h"

#ifndef __NENA_VIDEOOBJECTPERC_INCLUDED__
#define __NENA_VIDEOOBJECTPERC_INCLUDED__

namespace Nena
{
	namespace Video
	{
		namespace Perc
		{
			namespace Tracking
			{
				struct Object : public Utility::BasicStatus<>
				{
					static UINT32 const QueryDefaultWorking = UINT_ERROR;
					static UINT32 const ReadFromFileConfiguration = UINT_ERROR - 1;

					typedef void Action;
					typedef HRESULT Result;
					typedef pxcStatus Status;
					typedef std::wstring String;

					BOOL Paused;
					BOOL Enabled;
					UINT32 DescIndex;
					String TrackerConfig;
					String ProfileConfig;
					PXCSession::ImplDesc Desc;
					PXCDCVTracker::ProfileInfo Profile;
					PXCSmartPtr<PXCDCVTracker> Tracker;

					Object(_In_ Capture *);
					Object(_In_ PXCSession *);

					Action Init(_In_ Capture *);
					Action Init(_In_ PXCSession *);
					Action PauseTracking(_In_ BOOL paused);
					Action SubmitRequirements(_Inout_ Capture *host);

					Action SetProfile(_In_ Sample *sample);
					Action SearchProfile(_In_opt_ UINT32 option = QueryDefaultWorking);

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

#endif // !__NENA_VIDEOOBJECTPERC_INCLUDED__