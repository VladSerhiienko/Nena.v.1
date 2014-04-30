#include "app.precompiled.h"
#include "itv.oasis.home.h"
#include "itv.oasis.air.h"

#include "itv.oasis.home.startscreen.h"
#include "itv.oasis.home.hub.h"
#include "itv.oasis.ui.dialog.h"
#include "itv.oasis.ui.pointers.h"

InteractiveTV::Project::Oasis::Home::Home()
	: Oasis::State(nullptr)
	, Feed( &remote_input )
{
	Name = _Oasis_origin;
	Name += "home/";

	auto start_screen = new Home::StartScreen( this );
	auto hub = new Home::Hub( this );
	auto pointers = new Ui::Pointers( this );

	auto dialog = new Ui::Dialog( this );
	dialog->Add( "./Assets/Media-Pause.png" );
	dialog->Add( "./Assets/User-Add.png" );
	dialog->Add( "./Assets/Gear.png" );
	dialog->Add( "./Assets/Debug .png" );
	dialog->Add( "./Assets/News.png" );
	dialog->Add( "./Assets/Media-Player.png" );
	dialog->Set( pointers );

	states.push_back( start_screen );
	states.push_back( hub );
	// ...

	// on top
	// ...
	states.push_back( dialog );
	states.push_back( pointers );

	//ss->CurrentStage = Home::StartScreen::kSuspended;
	start_screen->Hub = hub;

	dialog->Resume( );
	pointers->Resume( );
	//dialog->Suspend( );

	Context = &Oasis::GetForCurrentThread()->Context;
	Overlay = &Context->Engine->Graphics.d2d;

	Context->Oasis->AssignId(this);
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"ctor"
		);
}

InteractiveTV::Project::Oasis::Home::~Home()
{
	Quit();
	for (auto &state : states)
		delete state,
		state = NULL;
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"dtor"
		);
}

void InteractiveTV::Project::Oasis::Home::Init()
{
	//remote_input.Init( );
	//remote_input.LaunchGestureTracking( );
	SetState( Remote::Input::kGestures );

	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	for (auto &state : states)
		state->Init();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"initted"
		);

	Initted(this);
}

void InteractiveTV::Project::Oasis::Home::Quit()
{
	remote_input.Quit( );

	for (auto &state : states)
		state->Quit();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo, 
		"quitted"
		);

	Quitted(this);
}

void InteractiveTV::Project::Oasis::Home::OnBeginFrameMove()
{
	Overlay->Context->SaveDrawingState(
		BlockState.Get()
		);
	Overlay->Context->BeginDraw(
		);
}

void InteractiveTV::Project::Oasis::Home::OnEndFrameMove()
{
	::HRESULT result(S_OK);

	result = Overlay->Context->EndDraw(
		);
	Overlay->Context->RestoreDrawingState(
		BlockState.Get()
		);
	if (FAILED(result)) _Oasis_air_grabot(
		this, OasisAirMsg::kError,
		"frame failed"
		);
}

void InteractiveTV::Project::Oasis::Home::OnFrameMove()
{
	Context->Web->OnFrameMove();

	/*if ( auto in =  BeginRead( ) )
	{
		if ( in->gestures [0].body > 0 )
		if ( in->gestures [0].label > 0 )
		{
			_Oasis_air_grabot( this, OasisAirMsg::kInfo,
				"gesture#0 %d (%d%%)", 
				in->gestures [0].label,
				in->gestures [0].confidence
				);

			if ( in->gestures [1].body > 0 )
			if ( in->gestures [1].label > 0 )
			{
				_Oasis_air_grabot( this, OasisAirMsg::kInfo,
					"gesture#1 %d (%d%%)",
					in->gestures [1].label,
					in->gestures [1].confidence
					);
			}
		}

		EndRead( );
	}*/

	OnBeginFrameMove();
	for ( auto &s : states )
		s->OnFrameMove( );
	OnEndFrameMove();
}

void InteractiveTV::Project::Oasis::Home::CreateDeviceResources()
{
	Overlay->D2DFactory->CreateDrawingStateBlock(
		BlockState.ReleaseAndGetAddressOf()
		);
	for (auto &state : states)
		state->CreateDeviceResources();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device resources discarded"
		);
}

void InteractiveTV::Project::Oasis::Home::CreateDeviceIndependentResources()
{
	for (auto &state : states)
		state->CreateDeviceIndependentResources();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-independent resources discarded"
		);
}

void InteractiveTV::Project::Oasis::Home::CreateWindowSizeDependentResources()
{
	for (auto &state : states)
		state->CreateWindowSizeDependentResources();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources discarded"
		);
}

void InteractiveTV::Project::Oasis::Home::DiscardDeviceResources()
{
	BlockState = nullptr;
	for (auto &state : states)
		state->DiscardDeviceResources();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependent resources discarded"
		);
}

void InteractiveTV::Project::Oasis::Home::DiscardWindowSizeDependentResources()
{
	for (auto &state : states)
		state->DiscardWindowSizeDependentResources();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources discarded"
		);
}

#pragma region Oasis State

void InteractiveTV::Project::Oasis::Home::Resume()
{
	for (auto &state : states)
		state->Resume();
	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"resumed"
		);

	Resumed(this);
}

void InteractiveTV::Project::Oasis::Home::Suspend()
{
	for (auto &state : states)
		state->Suspend();
	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"suspended"
		);

	Suspended(this);
}

void InteractiveTV::Project::Oasis::Home::OnResized()
{
	for (auto &state : states)
		state->OnResized();
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo, 
		"resized"
		);
}

void InteractiveTV::Project::Oasis::Home::OnKeyPressed(::UINT32 k)
{
}

void InteractiveTV::Project::Oasis::Home::OnKeyReleased(::UINT32 k)
{
}

void InteractiveTV::Project::Oasis::Home::OnMouseMoved(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::OnMouseLBPressed(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::OnMouseRBPressed(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::OnMouseLBReleased(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::OnMouseRBReleased(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::OnStateQuitted(_In_ Oasis::State *kid)
{
}

void InteractiveTV::Project::Oasis::Home::OnStateResumed(_In_ Oasis::State *kid)
{
}

void InteractiveTV::Project::Oasis::Home::OnStateSuspended(_In_ Oasis::State *kid)
{
}

#pragma endregion