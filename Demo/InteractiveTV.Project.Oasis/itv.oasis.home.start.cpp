#include "app.precompiled.h"
#include "itv.oasis.home.start.h"
#include "itv.oasis.air.h"

#define _Oasis_home_start "home/start/"

InteractiveTV::Oasis::Home::StartScreen::Stage
InteractiveTV::Oasis::Home::StartScreen::GetEasingStage(
)
{
	return Easing.CurrentStage;
}

void InteractiveTV::Oasis::Home::StartScreen::TextLabel::Init( )
{
	Txt.AcquireGlobals( );
}

void InteractiveTV::Oasis::Home::StartScreen::TextLabel::Quit( )
{
}

void InteractiveTV::Oasis::Home::StartScreen::TextLabel::CreateDeviceResources( )
{
}

void InteractiveTV::Oasis::Home::StartScreen::TextLabel::CreateWindowSizeDependentResources( )
{
	HRESULT result( S_OK );
	FontSizeOrigin = Txt.screen_size->Height * 0.125f;

	if ( !Txt.overlay->DWFactory.Get( ) ||
		!Txt.overlay->Context.Get( ) )
		return;
	result = Txt.overlay->DWFactory->CreateTextFormat(
		L"Segoe UI Light", NULL,
		DWRITE_FONT_WEIGHT_ULTRA_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		FontSizeOrigin,
		L"en-us",
		Txt.text_format.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) ) return;
	result = Txt.text_format->SetTextAlignment(
		DWRITE_TEXT_ALIGNMENT_LEADING
		);
	if ( FAILED( result ) ) return;
	result = Txt.text_format->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR
		);
	if ( FAILED( result ) ) return;
	result = Txt.text_format->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_UNIFORM,
		130.0f, 80.0f
		);
	if ( FAILED( result ) ) return;
	result = Txt.overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ),
		Txt.text_brush.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) ) return;
}

void InteractiveTV::Oasis::Home::StartScreen::TextLabel::OnFrameMove( )
{

	static String::size_type s_bytes =
		ARRAYSIZE( TimeTextBuffer ) *
		sizeof String::value_type;

	HRESULT result( S_OK );

	::time(
		&Timeux
		);
	::localtime_s(
		&Time, &Timeux
		);
	::wcsftime(
		TimeTextBuffer, s_bytes,
		L"%H:%M\n%A, %#d. %B",
		&Time
		);

	TimeText = TimeTextBuffer;

	auto start_point = D2D1::Point2F( 50, 50 );
	result = Txt.overlay->DWFactory->CreateTextLayout(
		TimeText.c_str( ),
		TimeText.size( ),
		Txt.text_format.Get( ),
		Txt.screen_size->Width - start_point.x,
		Txt.screen_size->Height - start_point.y,
		Txt.text_layout.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) )
		return;

	DWRITE_TEXT_RANGE time_range = { 0, 5 };
	Txt.text_layout->SetFontSize(
		FontSizeOrigin * 2.f,
		time_range
		);

	/*
	DWRITE_TEXT_RANGE hours_range = { 0, 2 };
	DWRITE_TEXT_RANGE minutes_range = { 3, 2 };
	TimeDateLabel.Txt.text_layout->SetFontWeight( DWRITE_FONT_WEIGHT_NORMAL, hours_range );
	TimeDateLabel.Txt.text_layout->SetFontSize( 200.0f, minutes_range );
	*/

	Txt.overlay->Context->DrawTextLayout(
		start_point, Txt.text_layout.Get( ),
		Txt.text_brush.Get( )
		);
}

InteractiveTV::Oasis::Home::StartScreen::StartScreen( Oasis::Home *master )
: Oasis::Home::State( master )
{
	Name = _Oasis_origin _Oasis_home_start;
	Context = &Oasis::GetForCurrentThread( )->Context;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"ctor"
		);
}

InteractiveTV::Oasis::Home::StartScreen::~StartScreen( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"dtor"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::Init( )
{
	TimeDateLabel.Init( );

	Navdialog.dialog_pointers = Pointers;
	Navdialog.AddButton( "./Assets/Media-Player.png" );
	Navdialog.AddButton( "./Assets/Business-Card-01.png" );
	Navdialog.AddButton( "./Assets/Debug .png" );
	Navdialog.AddButton( "./Assets/QR-Code.png" );
	Navdialog.AddButton( "./Assets/News.png" );
	Navdialog.AddButton( "./Assets/Confused.png" );
	Navdialog.Init( );

	Navdialog.button_selected += Nena_Delegate_MakeBind(
		this, &StartScreen::OnDialogButtonSelected
		);

	Easing.SetTotal( 2.0f );
	Easing.SuspendingValue.Params.Distance = 0.0f;
	Easing.SuspendingValue.Params.Offset = 2.0f;
	Easing.ResumingValue.Params.Distance = -2.0f;
	Easing.ResumingValue.Params.Offset = 2.0f;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"init"
		);

	Initted( this );
}

void InteractiveTV::Oasis::Home::StartScreen::Quit( )
{
	TimeDateLabel.Quit( );
	Navdialog.Quit( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"quit"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::CreateDeviceResources( )
{
	TimeDateLabel.CreateDeviceResources( );
	Navdialog.CreateDeviceResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependent resources created"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::OnFrameMove( )
{
	if ( GetEasingStage( ) == StartScreen::Stage::kSuspended )
		return;

	::Nena::Vector2 transaction_scene;
	Easing += TimeDateLabel.Txt.cross_context->Timer.Elapsed;

	transaction_scene.x = Easing.OnFrame(
		);
	TimeDateLabel.Txt.CalculateActualPositionMetricTs(
		transaction_scene
		);
	TimeDateLabel.Txt.overlay->Context->SetTransform(
		::D2D1::Matrix3x2F::Translation( { transaction_scene.x, 0 } )
		);
	TimeDateLabel.OnFrameMove(
		);
	TimeDateLabel.Txt.RestoreDeviceTransform(
		);
	Navdialog.OnFrameMove(
		);
}

void InteractiveTV::Oasis::Home::StartScreen::OnResized( )
{
	CreateWindowSizeDependentResources( );
	Navdialog.OnResized( );
}

void InteractiveTV::Oasis::Home::StartScreen::CreateDeviceIndependentResources( )
{
	Navdialog.CreateDeviceIndependentResources( );
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-independent resources created"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::CreateWindowSizeDependentResources( )
{
	TimeDateLabel.CreateWindowSizeDependentResources( );
	Navdialog.CreateWindowSizeDependentResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources created"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::DiscardDeviceResources( )
{
	Navdialog.DiscardDeviceResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependent resources discarded"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::DiscardWindowSizeDependentResources( )
{
	Navdialog.DiscardWindowSizeDependentResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources discarded"
		);
}

void InteractiveTV::Oasis::Home::StartScreen::Resume( )
{
	Easing.Resume( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"resumed"
		);

	Resumed( this );
}

void InteractiveTV::Oasis::Home::StartScreen::Suspend( )
{
	Easing.Suspend( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"suspended"
		);

	Suspended( this );
}


void InteractiveTV::Oasis::Home::StartScreen::OnDialogButtonSelected(
	::InteractiveTV::Ui::Round::Dialog *dlg,
	::InteractiveTV::Ui::Round::Dialog::Button *btn
	)
{
	if ( btn->button_id == 4 )
	{
		Hub->Resume( );
		Suspend( );
	}
}

void InteractiveTV::Oasis::Home::StartScreen::OnGestureReceived(
	Remote::Input::GestureAppMessageArg arg
	)
{
	if ( GetEasingStage( ) != Stage::kSuspended )
	{
		if ( Navdialog.GetEasingStage( ) == ::Nena::Animation::EasingStage::kResumed )
			Navdialog.OnGestureReceived( arg );

		switch ( arg )
		{
		case Remote::Input::kWave:
			break;
		case Remote::Input::kCircle:
			if ( Navdialog.GetEasingStage( ) == ::Nena::Animation::EasingStage::kSuspended )
				Navdialog.Resume( );

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
}

