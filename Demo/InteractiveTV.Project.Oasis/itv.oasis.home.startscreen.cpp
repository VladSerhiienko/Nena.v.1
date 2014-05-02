#include "app.precompiled.h"
#include "itv.oasis.home.startscreen.h"
#include "itv.oasis.air.h"

#define _Oasis_home_start "home/start/"

InteractiveTV::Project::Oasis::Home::StartScreen::StartScreen( Oasis::Home *master )
: Oasis::Home::State( master )
, Navigation( master )
{
	Name = _Oasis_origin _Oasis_home_start;
	Context = &Oasis::GetForCurrentThread( )->Context;
	Overlay = &Context->Engine->Graphics.d2d;

	CurrentStage = StartScreen::kSuspended;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"ctor"
		);
}

InteractiveTV::Project::Oasis::Home::StartScreen::~StartScreen( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"dtor"
		);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Init( )
{
	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );

	Navigation.Add( "./Assets/Media-Player.png" );
	Navigation.Add( "./Assets/User-Add.png" );
	Navigation.Add( "./Assets/Debug .png" );
	Navigation.Add( "./Assets/News.png" );
	Navigation.Set( Pointers );
	Navigation.ButtonSelected += Nena_Delegate_MakeBind(
		this, &StartScreen::OnButtonSelected
		);
	Navigation.Init( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"init"
		);

	Initted( this );
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Quit( )
{
	Navigation.Quit( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"quit"
		);

	Quitted( this );
}

void InteractiveTV::Project::Oasis::Home::StartScreen::CreateDeviceResources( )
{
	Navigation.CreateDeviceResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependent resources created"
		);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnFrameMove( )
{
	if ( CurrentStage == kSuspended )
		return;

	HRESULT result( S_OK );
	OnUpdateTimeText( );

	::Nena::Graphics::Resources::DirectWriteTextLayout layout;

	auto start_point = D2D1::Point2F( 50, 50 );
	result = Overlay->DWFactory->CreateTextLayout(
		TimeText.c_str( ), TimeText.size( ), TextFormat.Get( ),
		ScreenSize->Width - start_point.x,
		ScreenSize->Height - start_point.y,
		layout.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"dw text layout"
			);

		return;
	}

	DWRITE_TEXT_RANGE time_range = { 0, 5 };
	layout->SetFontSize( FontSizeOrigin * 1.3f, time_range );

	/*
	DWRITE_TEXT_RANGE hours_range = { 0, 2 };
	DWRITE_TEXT_RANGE minutes_range = { 3, 2 };
	layout->SetFontWeight( DWRITE_FONT_WEIGHT_NORMAL, hours_range );
	layout->SetFontSize( 200.0f, minutes_range );
	*/

	::FLOAT dx = 0.0f;
	switch ( CurrentStage )
	{
	case StartScreen::kSuspending:
		Easing.SuspendingX += Context->Timer.Elapsed;
		dx = Easing.SuspendingX.OnFrame( );
		break;
	case StartScreen::kResuming:
		Easing.ResumingX += Context->Timer.Elapsed;
		dx = Easing.ResumingX.OnFrame( );
		break;
	}

	Overlay->Context->SetTransform(
		D2D1::Matrix3x2F::Translation( { dx, 0 } )
		);
	Overlay->Context->DrawTextLayout(
		start_point, layout.Get( ),
		TextBrush.Get( )
		);
	Overlay->Context->SetTransform(
		D2D1::Matrix3x2F::Identity( )
		);
	Navigation.OnFrameMove(
		);


	switch ( CurrentStage )
	{
	case StartScreen::kSuspending:
		if ( Easing.SuspendingX.Saturated )
			Easing.SuspendingX.Reset( ),
			Easing.ResumingX.Reset( ),
			CurrentStage = kSuspended;
		break;
	case StartScreen::kResuming:
		if ( Easing.ResumingX.Saturated )
			Easing.SuspendingX.Reset( ),
			Easing.ResumingX.Reset( ),
			CurrentStage = kResumed;
		break;
	case StartScreen::kResumed:

		break;
	}
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnUpdateTimeText( )
{
	static String::size_type s_bytes =
		ARRAYSIZE( TimeTextBuffer ) *
		sizeof String::value_type;

	::time(
		&Timeux
		);
	::localtime_s(
		&Time, &Timeux
		);
	::wcsftime(
		TimeTextBuffer, s_bytes,
		L"%H:%M\n%A %d %B",
		&Time
		);

	TimeText = TimeTextBuffer;
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnResized( )
{
	CreateWindowSizeDependentResources( );
}

void InteractiveTV::Project::Oasis::Home::StartScreen::CreateDeviceIndependentResources( )
{
	Navigation.CreateDeviceIndependentResources( );


	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-independent resources created"
		);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::CreateWindowSizeDependentResources( )
{
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;
	FontSizeOrigin = ScreenSize->Height * 0.125f;

	HRESULT result( S_OK );

	if ( !Overlay->DWFactory.Get( ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"dw factory"
			);

		return;
	}
	result = Overlay->DWFactory->CreateTextFormat(
		L"Segoe UI Light", NULL,
		DWRITE_FONT_WEIGHT_ULTRA_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		FontSizeOrigin,
		L"en-us",
		TextFormat.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"dw text format"
			);

		return;
	}
	result = TextFormat->SetTextAlignment(
		DWRITE_TEXT_ALIGNMENT_LEADING
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"dw text alignment"
			);

		return;
	}
	result = TextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"dw text para-alignment"
			);

		return;
	}
	result = TextFormat->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_UNIFORM,
		130.0f, 80.0f
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"dw text line spacing"
			);

		return;
	}
	result = Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ),
		TextBrush.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kCritical,
			"solid brush"
			);

		return;
	}

	auto total = 1.0f;
	auto susp_factor = 0.2f;

	Easing.ResumingX.Params.Total = total;
	Easing.ResumingX.Params.Elapsed = 0.0f;
	Easing.ResumingX.Params.Offset = ScreenSize->Width;
	Easing.ResumingX.Params.Distance = -ScreenSize->Width;

	Easing.SuspendingX.Params.Total = total * susp_factor;
	Easing.SuspendingX.Params.Elapsed = 0.0f;
	Easing.SuspendingX.Params.Offset = 0.0f;
	Easing.SuspendingX.Params.Distance = -ScreenSize->Width;

	Navigation.CreateWindowSizeDependentResources( );


	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources created"
		);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::DiscardDeviceResources( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependent resources discarded"
		);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::DiscardWindowSizeDependentResources( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources discarded"
		);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Resume( )
{
	CurrentStage = StartScreen::kResuming;
	Easing.SuspendingX.Reset( );
	Easing.ResumingX.Reset( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"resumed"
		);

	Resumed( this );
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Suspend( )
{
	CurrentStage = StartScreen::kSuspending;
	Easing.SuspendingX.Reset( );
	Easing.ResumingX.Reset( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"suspended"
		);

	Suspended( this );
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnButtonSelected(
	Ui::Dialog *navigation, Ui::Button *selection
	)
{
	if ( navigation == &Navigation && selection ) switch ( selection->Index )
	{
		case 3:
			Suspend( );
			Hub->Resume( );
			break;
	} 
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnGestureReceived(
	Remote::Input::GestureAppMessageArg arg
	)
{
	if ( CurrentStage != StartScreen::kResumed ) return;
	else if ( Navigation.CurrentStage == Ui::Dialog::kResumed )
		Navigation.OnGestureReceived( arg );
	else switch ( arg )
	{
	case Remote::Input::kWave:
		break;
	case Remote::Input::kCircle:

		if ( Navigation.CurrentStage == Ui::Dialog::kSuspended )
			Navigation.Resume( );

		break;
	case Remote::Input::kNavigationSwipeUp:
		break;
	case Remote::Input::kNavigationSwipeDown:
		break;
	case Remote::Input::kNavigationSwipeLeft:
		break;
	case Remote::Input::kNavigationSwipeRight:
		break;
	}
}

