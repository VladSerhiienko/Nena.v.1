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
	static UINT32 const GestureAppMessage = WM_USER + 2;
	typedef Nena::Event<void, Remote::Input *> Event;

	typedef enum GestureAppMessageArg
	{
		kWave = PXCGesture::Gesture::LABEL_HAND_WAVE,
		kCircle = PXCGesture::Gesture::LABEL_HAND_CIRCLE,
		kNavigationSwipeUp = PXCGesture::Gesture::LABEL_NAV_SWIPE_UP,
		kNavigationSwipeDown = PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN,
		kNavigationSwipeLeft = PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT,
		kNavigationSwipeRight = PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT,

	} GestureAppMessageArg;

	typedef enum FeedTarget : UINT32
	{
		kIdle = 0,
		kGestures = 1 << 0,
		kFaceDetection = 1 << 1,
		kFaceDetectionIdle = 1 << 2,
		kFaceRecognition = 1 << 3,

	} FeedTarget;

	struct GestureCallback :
		protected PXCGesture::Gesture::Handler,
		protected PXCGesture::Alert::Handler
	{
		GestureCallback( Remote::Input * );

		typedef PXCGesture::Alert::Handler *HAlert;
		typedef PXCGesture::Gesture::Handler *HGesture;
		inline HAlert GetAlertHandler( ) { return this; }
		inline HGesture GetGestureHandler( ) { return this; }
		virtual void PXCAPI OnAlert( _In_ PXCGesture::Alert * ) override;
		virtual void PXCAPI OnGesture( _In_ PXCGesture::Gesture * ) override;

	private:

		Remote::Input *m_host;
	};

	typedef struct Details
	{
		PXCGesture::GeoNode Nodes[ 2 ][ 11 ];
	} Details;

	typedef struct Feed
	{
		Feed( Remote::Input * );
		void Set( Remote::Input::FeedTarget );
		::BOOL IsHostValid( );

		Remote::Input::Details *BeginRead( );
		void EndRead( );

	private:

		friend Remote::Input;
		Remote::Input::Details m_queried_remote_input;
		Remote::Input *m_host;

	} Feed;

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

	volatile Input::FeedTarget m_feed_tgt;
	volatile ::BOOL m_is_ready;
	volatile ::BOOL m_is_running;
	volatile ::BOOL m_is_feed_busy;
	volatile ::BOOL m_is_busy;
	volatile ::BOOL m_is_valid;

	Input::Details m_remote_input_details;
	concurrency::task_group m_tracking_flow;
	concurrency::task_group_status m_tracking_status;
	Nena::Simulation::BasicTimer m_remote_input_timer;

	void GestureTrackingRoutine( );
	static void QueryGestureData( Details *, PXCGesture * );
	static void PrintGestureData( PXCGesture * );

};

#endif // !__NENA_INTERACTIVE_TV_REMOTE_CONTROL_INCLUDED__


