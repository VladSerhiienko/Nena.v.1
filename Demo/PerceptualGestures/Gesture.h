
#include "Precompiled.h"
#include "Streamer.h"

#ifndef __NENA_PERCEPTUAL_VIDEOPROCGESTURE_INCLUDED__
#define __NENA_PERCEPTUAL_VIDEOPROCGESTURE_INCLUDED__

namespace Nena
{
	namespace Video
	{
		struct Streamer;
		namespace Processing
		{
			struct Gesture : 
				public PXCGesture::Gesture::Handler,
				public PXCGesture::Alert::Handler
			{
				typedef HRESULT OnProcessCallback(Gesture *);
				typedef void OnGestureCallback(Gesture *, PXCGesture::Gesture *);
				typedef void OnAlertCallback(Gesture *, PXCGesture::Alert *);
				typedef PXCGesture::ProfileInfo ProfileInfo;
				typedef std::list<ProfileInfo> ProfileList;
				typedef pxcStatus Status;

				template <typename _Callback> void SetCallback(_Callback *callback);
				
				template <> void SetCallback<OnProcessCallback>(OnProcessCallback *callback)
				{ if (callback) OnProcessTrackingResults = callback; }
				template <> void SetCallback<OnGestureCallback>(OnGestureCallback *callback) 
				{ if (callback) OnGestureCall = callback; }
				template <> void SetCallback<OnAlertCallback>(OnAlertCallback *callback) 
				{ if (callback) OnAlertCall = callback; }

				PXCSmartPtr<PXCGesture> Base;
				Streamer *Host;

				Sampler::Sample *Sample;
				Sampler Sampler;

				PXCGesture::ProfileInfo Profile;
				ProfileList ScannedProfiles;
				PXCSession::ImplDesc Desc;

				Gesture(Streamer &host);
				Gesture(Streamer *host);

				HRESULT Init(BOOL recreate = FALSE);
				void Close();

				static void MissingOnGestureCallback(Gesture *gestureInstance, PXCGesture::Gesture *gestureData);
				static void MissingOnAlertCallback(Gesture *gestureInstance, PXCGesture::Alert *alertData);
				static HRESULT MissingOnProcessCallback(Gesture *gestureInstance);
				virtual void PXCAPI OnGesture(PXCGesture::Gesture *gesture);
				virtual void PXCAPI OnAlert(PXCGesture::Alert *alert);

				void SetProfile(ProfileInfo *profile = nullptr); // Sets profile for gesture base
				UINT32 GetRequiredStreamCount(); // Profile should be set
				void ScanProfiles(); // Get all possible gesture profiles
				void CleanProfiles(); // Erases all profiles from the list
				HRESULT LocateStreams(); // Configures provided streamer instance

				HRESULT SubscribeAlert();
				HRESULT UnsubscribeAlert();
				HRESULT SubscribeGesture();
				HRESULT UnsubscribeGesture();
				HRESULT OnFrameMove();
				HRESULT OnFrameMove(PXCImage *images[], PXCScheduler::SyncPoint *syncs[]);

			protected:

				// threshold must always be 100 (to subscribe) 
				// or 0 (to unsubscribe) gesture recognizion

				static pxcU32 const ThresholdMax = 100;
				static pxcU32 const ThresholdMin = 0;

				OnGestureCallback *OnGestureCall;
				OnAlertCallback *OnAlertCall;
				OnProcessCallback *OnProcessTrackingResults;

			};
		}

	}
}

#endif //!__NENA_PERCEPTUAL_VIDEOPROCGESTURE_INCLUDED__