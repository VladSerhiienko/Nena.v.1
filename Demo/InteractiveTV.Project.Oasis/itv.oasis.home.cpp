#include "app.precompiled.h"

#include "itv.oasis.home.h"
#include "itv.oasis.web.h"
#include "itv.oasis.air.h"

#include "itv.oasis.home.start.h"
#include "itv.oasis.home.hub.h"

InteractiveTV::Oasis::Home::Home( )
: Oasis::State( nullptr )
, Feed( &remote_input )
{
	Name = _Oasis_origin;
	Name += "home/";

	auto hub = new HubScreen( this );
	auto start = new StartScreen( this );
	auto pointers = new Ui::Pointers( this );
	start->Pointers = pointers;
	hub->Pointers = pointers;
	start->Hub = hub;
	hub->Start = start;

	states.push_back( hub );
	states.push_back( start );
	states.push_back( pointers );

	hub->Resume( );
	pointers->Resume( );

	Context = &Oasis::GetForCurrentThread( )->Context;
	Overlay = &Context->Engine->Graphics.d2d;

	Context->Oasis->AssignId( this );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"ctor"
		);
}

InteractiveTV::Oasis::Home::~Home( )
{
	Quit( );
	for ( auto &state : states )
		delete state,
		state = NULL;
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"dtor"
		);
}

void InteractiveTV::Oasis::Home::Init( )
{
	remote_input.Init( );
	remote_input.LaunchGestureTracking( );
	Set( Remote::Input::kGestures );

	for ( auto &state : states )
		state->Init( );

	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"initted"
		);

	Initted( this );
}

void InteractiveTV::Oasis::Home::Quit( )
{
	remote_input.Quit( );
	for ( auto &state : states )
		state->Quit( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"quitted"
		);

	Quitted( this );
}

void InteractiveTV::Oasis::Home::OnBeginFrameMove( )
{
	Overlay->Context->SaveDrawingState(
		BlockState.Get( )
		);
	Overlay->Context->BeginDraw(
		);
}

void InteractiveTV::Oasis::Home::OnEndFrameMove( )
{
	::HRESULT result( S_OK );

	result = Overlay->Context->EndDraw(
		);
	Overlay->Context->RestoreDrawingState(
		BlockState.Get( )
		);
	if ( FAILED( result ) ) _Oasis_air_grabot(
		this, OasisAirMsg::kError,
		"frame failed"
		);
}

void InteractiveTV::Oasis::Home::OnFrameMove( )
{
	Context->Web->OnFrameMove( );

	OnBeginFrameMove( );
	for ( auto &s : states )
		s->OnFrameMove( );
	OnEndFrameMove( );
}

void InteractiveTV::Oasis::Home::CreateDeviceResources( )
{
	Overlay->D2DFactory->CreateDrawingStateBlock(
		BlockState.ReleaseAndGetAddressOf( )
		);
	for ( auto &state : states )
		state->CreateDeviceResources( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device resources discarded"
		);
}

void InteractiveTV::Oasis::Home::CreateDeviceIndependentResources( )
{
	for ( auto &state : states )
		state->CreateDeviceIndependentResources( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-independent resources discarded"
		);
}

void InteractiveTV::Oasis::Home::CreateWindowSizeDependentResources( )
{
	for ( auto &state : states )
		state->CreateWindowSizeDependentResources( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources discarded"
		);
}

void InteractiveTV::Oasis::Home::DiscardDeviceResources( )
{
	BlockState = nullptr;
	for ( auto &state : states )
		state->DiscardDeviceResources( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependent resources discarded"
		);
}

void InteractiveTV::Oasis::Home::DiscardWindowSizeDependentResources( )
{
	for ( auto &state : states )
		state->DiscardWindowSizeDependentResources( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources discarded"
		);
}

#pragma region Oasis State

void InteractiveTV::Oasis::Home::Resume( )
{
	for ( auto &state : states )
		state->Resume( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"resumed"
		);

	Resumed( this );
}

void InteractiveTV::Oasis::Home::Suspend( )
{
	for ( auto &state : states )
		state->Suspend( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"suspended"
		);

	Suspended( this );
}

void InteractiveTV::Oasis::Home::OnResized( )
{
	for ( auto &state : states )
		state->OnResized( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"resized"
		);
}

void InteractiveTV::Oasis::Home::OnKeyPressed( ::UINT32 k )
{
}

void InteractiveTV::Oasis::Home::OnKeyReleased( ::UINT32 k )
{
	/*
	switch ( g )
	{
	case InteractiveTV::Remote::Input::kWave: OnKeyReleased( Nena::Application::VirtualKey::W ); break;
	case InteractiveTV::Remote::Input::kCircle: OnKeyReleased( Nena::Application::VirtualKey::C ); break;
	case InteractiveTV::Remote::Input::kNavigationSwipeUp: OnKeyReleased( Nena::Application::VirtualKey::Up ); break;
	case InteractiveTV::Remote::Input::kNavigationSwipeDown: OnKeyReleased( Nena::Application::VirtualKey::Down ); break;
	case InteractiveTV::Remote::Input::kNavigationSwipeLeft: OnKeyReleased( Nena::Application::VirtualKey::Left ); break;
	case InteractiveTV::Remote::Input::kNavigationSwipeRight: OnKeyReleased( Nena::Application::VirtualKey::Right ); break;
	}
	*/
}

void InteractiveTV::Oasis::Home::OnGestureReceived( _In_::UINT32 g )
{
	for ( auto &state : states )
		state->OnGestureReceived( 
			(Remote::Input::GestureAppMessageArg) g 
			);
}

void InteractiveTV::Oasis::Home::OnMouseMoved( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::OnMouseLBPressed( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::OnMouseRBPressed( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::OnMouseLBReleased( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::OnMouseRBReleased( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::OnStateQuitted( _In_ Oasis::State *kid )
{
}

void InteractiveTV::Oasis::Home::OnStateResumed( _In_ Oasis::State *kid )
{
}

void InteractiveTV::Oasis::Home::OnStateSuspended( _In_ Oasis::State *kid )
{
}

#pragma endregion