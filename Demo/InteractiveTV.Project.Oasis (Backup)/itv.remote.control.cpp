#include "app.precompiled.h"
#include "itv.remote.control.h"
#include "itv.oasis.air.h"

#define _Oasis_remote_input "input/remote/"

InteractiveTV::Remote::Input::GestureCallback::GestureCallback(
	Remote::Input *host
	)
	: m_host( host )
{
}

void InteractiveTV::Remote::Input::GestureCallback::OnAlert( _In_ PXCGesture::Alert *alert )
{
}

void InteractiveTV::Remote::Input::GestureCallback::OnGesture( _In_ PXCGesture::Gesture *data )
{
	static auto app = InteractiveTV::Project::Oasis::GetForCurrentThread( )->Context.App;

	switch ( data->label )
	{
	case PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT:
	case PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT:
	case PXCGesture::Gesture::LABEL_NAV_SWIPE_UP:
	case PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN:
	case PXCGesture::Gesture::LABEL_HAND_CIRCLE:
	case PXCGesture::Gesture::LABEL_HAND_WAVE:

		app->PostUserMessage(
			GestureAppMessage,
			data->confidence,
			data->label
			);
	}
}

InteractiveTV::Remote::Input::Input(
	)
	: GestureTracker( &VideoCapturer )
	, GestureTrackerOutput( this )
	, m_is_ready( FALSE )
	, m_is_valid( TRUE )
	, m_feed_tgt( FeedTarget::kIdle )
{
	Name = _Oasis_origin _Oasis_remote_input;
}

InteractiveTV::Remote::Input::~Input( )
{
	Quit( );
}

void InteractiveTV::Remote::Input::Init( )
{
	if ( m_is_ready ) return;
	concurrency::create_task( [ this ] ( )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical,
			"initialization (in progress)"
			);

		m_is_ready = FALSE;

		if ( !VideoCapturer.IsValid( ) )
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"capture (invalid %d)",
				VideoCapturer.LastStatus
				);

			return FALSE;
		}

		if ( !GestureTracker.IsValid( ) )
		{
			m_is_valid = FALSE;
			_Oasis_air_grabot( this, OasisAirMsg::kCritical,
				"tracker (invalid %d)",
				GestureTracker.LastStatus
				);

			return FALSE;
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

			return FALSE;
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

			return FALSE;
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

			return FALSE;
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

			return FALSE;
		}

		GestureTracker.Subscribe( GestureTrackerOutput.GetGestureHandler( ) );
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

			return FALSE;
		}

		return TRUE;
	} ).then( [ this ] ( BOOL result )
	{
		m_is_busy = TRUE;
		m_is_valid = result;
		m_is_ready = result;

		_Oasis_air_grabot( this, OasisAirMsg::kInfo,
			"initialzion (succeeded)"
			);
	} );
}

::BOOL InteractiveTV::Remote::Input::IsReady( )
{
	return m_is_ready;
}

::BOOL InteractiveTV::Remote::Input::IsRunning( )
{
	return m_is_running;
}

void InteractiveTV::Remote::Input::Quit( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"quit"
		);
	if ( m_is_running ) CancelGestureTracking( );
}

void InteractiveTV::Remote::Input::LaunchGestureTracking( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"submitting launch..."
		);
	m_tracking_flow.run( [ this ] { GestureTrackingRoutine( ); } );
}

void InteractiveTV::Remote::Input::CancelGestureTracking( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"cancelling tracking..."
		);

	m_tracking_flow.cancel( );
	m_tracking_flow.wait( );
}

void InteractiveTV::Remote::Input::QueryGestureData(
	Details *data, PXCGesture *gesture
	)
{
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, 10, data->Nodes[ 0 ] );
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, 10, data->Nodes[ 1 ] );
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_ELBOW_PRIMARY, &data->Nodes[ 0 ][ 10 ] );
	gesture->QueryNodeData( 0, PXCGesture::GeoNode::LABEL_BODY_ELBOW_SECONDARY, &data->Nodes[ 1 ][ 10 ] );
	//gesture->QueryGestureData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, 0, &data->gestures[ 0 ] );
	//gesture->QueryGestureData( 0, PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, 0, &data->gestures[ 1 ] );
}

void InteractiveTV::Remote::Input::GestureTrackingRoutine( )
{
	pxcStatus sts;
	PXCSmartSPArray sps( 2 );
	PXCSmartArray<PXCImage> images;

	m_remote_input_timer.Reset( );
	m_is_running = TRUE;
	m_is_busy = TRUE;
	m_is_feed_busy = FALSE;

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
		if ( m_feed_tgt == FeedTarget::kIdle )
		{
			::Sleep( 1000 );
		}
		else
		{
			if ( (m_feed_tgt & FeedTarget::kGestures) == FeedTarget::kGestures )
			{
				if ( !m_is_feed_busy )
				{
					sts = VideoSampler->SubmitReadTask( images.ReleaseRefs( ), sps.ReleaseRef( 0 ) );
					if ( Nena::Video::Utility::Failed( sts ) ) break;
					sts = GestureTracker.SubmitProcessTask( images, sps.ReleaseRef( 1 ) );
					if ( Nena::Video::Utility::Failed( sts ) ) break;
					sts = sps.SynchronizeEx( );
					if ( Nena::Video::Utility::Failed( sts ) ) break;

					m_is_busy = TRUE;
					QueryGestureData( &m_remote_input_details, GestureTracker.Base );
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

		m_remote_input_timer.Update( );
	}

	m_is_running = FALSE;

	_Oasis_air_grabot( this, OasisAirMsg::kInfo,
		"off"
		);
}

InteractiveTV::Remote::Input::Feed::Feed(
	Remote::Input *host
	)
	: m_host( host )
{
	ZeroMemory( &m_queried_remote_input, sizeof m_queried_remote_input );
}

InteractiveTV::Remote::Input::Details *InteractiveTV::Remote::Input::Feed::BeginRead(
	)
{
	if ( m_host->m_is_busy ||
		// assert with msg "@see EndRead()"
		// also, reads from different threads are not allowed
		// current, only one feed per remote input is allowed
		m_host->m_is_feed_busy ||
		!m_host->m_is_running )
		return nullptr;
	else
	{
		m_host->m_is_feed_busy = TRUE;
		::memcpy(
			&m_queried_remote_input,
			&m_host->m_remote_input_details,
			sizeof m_queried_remote_input
			);

		return &m_queried_remote_input;
	}
}

void InteractiveTV::Remote::Input::Feed::EndRead(
	)
{
	ZeroMemory( &m_queried_remote_input, sizeof m_queried_remote_input );
	m_host->m_is_feed_busy = FALSE;
}

void InteractiveTV::Remote::Input::Feed::Set(
	Remote::Input::FeedTarget feed_tgt
	)
{
	m_host->m_feed_tgt = feed_tgt;
}
