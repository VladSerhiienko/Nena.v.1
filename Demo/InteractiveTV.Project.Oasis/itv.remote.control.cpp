#include "app.precompiled.h"
#include "itv.remote.control.h"
#include "itv.oasis.air.h"

void InteractiveTV::Remote::Input::GestureCallback::OnAlert( _In_ PXCGesture::Alert *alert )
{
}

void InteractiveTV::Remote::Input::GestureCallback::OnGesture( _In_ PXCGesture::Gesture *data )
{
	using namespace Nena::Video;
	if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-swipe-left\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-swipe-right\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-swipe-down\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_UP ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-swipe-up\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_HAND_CIRCLE ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-circle\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_POSE_BIG5 ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-5\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_POSE_PEACE ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-2\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_POSE_THUMB_UP ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-1\n";
	else if ( Nena::Video::Utility::Equals( data->label, PXCGesture::Gesture::LABEL_HAND_WAVE ) )
		std::cout << "[!]/.//oasis/input/remote/event: gesture-wave\n";
}

InteractiveTV::Remote::Input::Input( )
	: GestureTracker( &VideoCapturer )
	, m_ready (FALSE)
	, m_is_valid( TRUE )
	, m_state( State::kIdle )
{
	Name = _Oasis_origin;
	Name += "input/remote/";
}

InteractiveTV::Remote::Input::~Input( )
{
	Quit( );
}

void InteractiveTV::Remote::Input::Init( )
{
	if ( m_ready ) return;
	concurrency::create_task( [ this ] ( )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical,
			"initialization (in progress)"
			);

		m_ready = FALSE;

		if ( !VideoCapturer.IsValid( ) )
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"capture (invalid %d)",
				VideoCapturer.LastStatus
				);
			return;
		}

		if ( !GestureTracker.IsValid( ) )
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"tracker (invalid %d)",
				GestureTracker.LastStatus
				);
			return;
		}
		
		GestureTracker.SearchProfile( );
		if ( GestureTracker.IsValid( ) )
		{
			_Oasis_air_grabot( this, OasisAirMsg::kInfo,
				"profiles search (succeeded)"
				);
		}
		else
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"profiles search (failed %d)", 
				GestureTracker.LastStatus
				);
			return;
		}

		GestureTracker.SubmitRequirements( &VideoCapturer );
		if ( GestureTracker.IsValid( ) )
		{
			_Oasis_air_grabot( this, OasisAirMsg::kInfo,
				"requirements submission (succeeded)"
				);
		}
		else
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"requirements submission (failed %d)", 
				GestureTracker.LastStatus
				);
			return;
		}

		VideoCapturer.LocateStreams( );
		if ( VideoCapturer.IsValid( ) )
		{
			_Oasis_air_grabot( this, OasisAirMsg::kInfo,
				"video streams location (succeeded)"
				);
		}
		else
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"video streams location (failed %d)",
				VideoCapturer.LastStatus
				);
			return;
		}

		GestureTracker.SetProfile( );
		if ( GestureTracker.IsValid( ) )
		{
			_Oasis_air_grabot( this, OasisAirMsg::kInfo,
				"setting stream profiles (succeeded)"
				);
		}
		else
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"setting stream profiles (failed %d)",
				GestureTracker.LastStatus
				);
			return;
		}

		//GestureTracker.Subscribe( GestureTrackerOutput.GetGestureHandler( ) );

		VideoSampler = VideoCapturer.GetSampleFromDesc( GestureTracker.DescIndex );
		if ( VideoSampler )
		{
			_Oasis_air_grabot( this, OasisAirMsg::kInfo,
				"samplers location (succeeded)"
				);
		}
		else
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"samplers location (failed)"
				);
			return;
		}



	} ).then( [ this ] ( )
	{
		m_is_valid = FALSE;
		_Oasis_air_grabot( this, OasisAirMsg::kInfo,
			"initialzion (succeeded)"
			);
		m_is_busy = TRUE;
		m_ready = TRUE;
	} );
}

::BOOL InteractiveTV::Remote::Input::IsReady( )
{
	return m_ready;
}

::BOOL InteractiveTV::Remote::Input::IsRunning( )
{
	return m_running;
}

void InteractiveTV::Remote::Input::Quit( )
{
	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"quit"
		);
	std::cout << "[!]/.//oasis/input/remote/quit... \n";
	if ( m_running ) CancelGestureTracking( );
}

void InteractiveTV::Remote::Input::LaunchGestureTracking( )
{
	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"submitting launch..."
		);
	m_flow.run( [ this ] { GestureTrackingRoutine( ); } );
}

void InteractiveTV::Remote::Input::CancelGestureTracking( )
{
	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"cancelling tracking..."
		);
	m_flow.cancel( );
	m_flow.wait( );
}

void InteractiveTV::Remote::Input::QueryGestureData( 
	Details *data, PXCGesture *gesture 
	)
{
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, 10, data->nodes[0] );
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, 10, data->nodes [1] );
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_ELBOW_PRIMARY, &data->nodes [0][10] );
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_ELBOW_SECONDARY, &data->nodes [1][10] );
	gesture->QueryGestureData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, 0, &data->gestures [0] );
	gesture->QueryGestureData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, 0, &data->gestures [1] );
}

void InteractiveTV::Remote::Input::GestureTrackingRoutine( )
{
	pxcStatus sts;
	PXCSmartSPArray sps( 2 );
	PXCSmartArray<PXCImage> images;

	timer.Reset( );
	m_running = TRUE;
	m_is_busy = TRUE;
	m_feed_is_busy = FALSE;

	while ( 
		!concurrency::is_current_task_group_canceling( ) &&
		!concurrency::is_task_cancellation_requested( ) &&
		!IsReady( ) 
		)
	{
		_Oasis_air_grabot( this, OasisAirMsg::kInfo,
			"waiting for device..."
			);
		::Sleep( 1500 );
	}

	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"device is ready"
		);
	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"on"
		);

	while ( 
		!concurrency::is_current_task_group_canceling( ) &&
		!concurrency::is_task_cancellation_requested( ) 
		)
	{
		if ( m_state == kIdle )
		{
			::Sleep( 1000 );
		}
		else
		{
			if ( m_state & kGestures == kGestures )
			{
				if ( !m_feed_is_busy )
				{
					sts = VideoSampler->SubmitReadTask( images.ReleaseRefs( ), sps.ReleaseRef( 0 ) );
					if ( Nena::Video::Utility::Failed( sts ) ) break;
					sts = GestureTracker.SubmitProcessTask( images, sps.ReleaseRef( 1 ) );
					if ( Nena::Video::Utility::Failed( sts ) ) break;
					sts = sps.SynchronizeEx( );
					if ( Nena::Video::Utility::Failed( sts ) ) break;

					m_is_busy = TRUE;
					QueryGestureData( &gesture_data, GestureTracker.Base );
					m_is_busy = FALSE;
				}
				else
				{
					_Oasis_air_grabot( this, OasisAirMsg::kWarning,
						"waiting for feed..."
						);
				}
			}
		}

		timer.Update( );
	}

	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"off"
		);
	m_running = FALSE;
}

InteractiveTV::Remote::Input::Feed::Feed( Remote::Input *host )
	: Host( host )
{
	std::cout << "[!]/.//oasis/input/remote/feed/ctor \n";
	ZeroMemory( &QueriedRemoteInput, sizeof QueriedRemoteInput );
}

InteractiveTV::Remote::Input::Details *InteractiveTV::Remote::Input::Feed::BeginRead( 
	)
{
	if ( Host->m_is_busy || !Host->m_running ) return nullptr;
	else
	{
		Host->m_feed_is_busy = TRUE;
		memcpy( 
			&QueriedRemoteInput, 
			&Host->gesture_data,
			sizeof QueriedRemoteInput
			);

		return &QueriedRemoteInput;
	}
}

void InteractiveTV::Remote::Input::Feed::EndRead(
	)
{
	ZeroMemory( &QueriedRemoteInput, sizeof QueriedRemoteInput );
	Host->m_feed_is_busy = FALSE;
}

void InteractiveTV::Remote::Input::Feed::SetState( 
	Remote::Input::State state
	)
{
	Host->m_state = state;
}
