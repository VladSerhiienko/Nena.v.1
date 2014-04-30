#include "app.precompiled.h"
#include "nena.basic.timer.h"
#include "itv.oasis.h"

#ifndef __NENA_INTERACTIVE_TV_REMOTE_CONTROL_INCLUDED__
#define __NENA_INTERACTIVE_TV_REMOTE_CONTROL_INCLUDED__

namespace InteractiveTV 
{ 
	namespace Remote
	{
		struct Input;
	}
}

struct InteractiveTV::Remote::Input : public InteractiveTV::Project::Oasis::Object
{
	enum State
	{
		kIdle = 0,
		kGestures = 1 << 0,
		kFaceDetection = 1 << 1,
		kFaceDetectionIdle = 1 << 2,
		kFaceRecognition = 1 << 3,
	};

	struct GestureCallback : 
		protected PXCGesture::Gesture::Handler,
		protected PXCGesture::Alert::Handler
	{
		typedef PXCGesture::Alert::Handler *HAlert;
		typedef PXCGesture::Gesture::Handler *HGesture;
		inline HAlert GetAlertHandler( ) { return this; }
		inline HGesture GetGestureHandler( ) { return this; }
		virtual void PXCAPI OnAlert( _In_ PXCGesture::Alert *alert ) override;
		virtual void PXCAPI OnGesture( _In_ PXCGesture::Gesture *data ) override;
	};

	struct Details
	{
		PXCGesture::Gesture gestures [2];
		PXCGesture::GeoNode nodes [2][11];
	};

	struct Feed
	{
		Feed( Remote::Input *host );
		void SetState( Remote::Input::State state );
		::BOOL IsHostValid( );

		Remote::Input::Details *BeginRead( );
		void EndRead( );

	private:

		friend Remote::Input;
		Remote::Input::Details QueriedRemoteInput;
		Remote::Input *Host;
	};

	GestureCallback GestureTrackerOutput;
	Nena::Video::Perc::Sample *VideoSampler;
	Nena::Video::Perc::Capture VideoCapturer;
	Nena::Video::Perc::Tracking::Gesture GestureTracker;

	Input( );
	~Input( );

	void Init( );
	void Quit( );
	void LaunchGestureTracking( );
	void CancelGestureTracking( );
	::BOOL IsReady( );
	::BOOL IsRunning( );

private:

	volatile State m_state;
	volatile ::BOOL m_ready;
	volatile ::BOOL m_running;
	volatile ::BOOL m_feed_is_busy;
	volatile ::BOOL m_is_busy;
	volatile ::BOOL m_is_valid;
	Details gesture_data;

	Nena::Simulation::BasicTimer timer;

	concurrency::task_group m_flow;
	concurrency::task_group_status m_trackingStatus;

	void GestureTrackingRoutine( );
	static void QueryGestureData( Details *data, PXCGesture *gesture );
	static void PrintGestureData( PXCGesture *gesture );

};

#endif // !__NENA_INTERACTIVE_TV_REMOTE_CONTROL_INCLUDED__


