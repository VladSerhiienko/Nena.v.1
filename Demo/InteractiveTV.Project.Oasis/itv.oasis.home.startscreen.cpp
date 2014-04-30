#include "app.precompiled.h"
#include "itv.oasis.home.startscreen.h"
#include "itv.oasis.air.h"


InteractiveTV::Project::Oasis::Home::StartScreen::StartScreen(Oasis::Home *master)
	: Oasis::Home::State(master)
{
	Name = _Oasis_origin;
	Name += "home/start/";

	Context = &Oasis::GetForCurrentThread()->Context;
	Overlay = &Context->Engine->Graphics.d2d;

	CurrentStage = kResuming;

	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "ctor");
}

InteractiveTV::Project::Oasis::Home::StartScreen::~StartScreen()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "dtor");
}


void InteractiveTV::Project::Oasis::Home::StartScreen::Init()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "init");

	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources( );

	TranslationEasing.ResumingX.Params.c = -ScreenSize->Width;
	TranslationEasing.ResumingX.Params.d = 1.0f;
	TranslationEasing.ResumingX.Params.t = 0.0f;
	TranslationEasing.ResumingX.Params.b = ScreenSize->Width;

	TranslationEasing.SuspendingX.Params.c = -ScreenSize->Width;
	TranslationEasing.SuspendingX.Params.d = 1.0f;
	TranslationEasing.SuspendingX.Params.t = 0.0f;
	TranslationEasing.SuspendingX.Params.b = 0.0f;

	//CurrentStage = kResuming;
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Quit()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "quit");

}

void InteractiveTV::Project::Oasis::Home::StartScreen::CreateDeviceResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "create device-dependent resources");

}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnFrameMove()
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
	//DWRITE_TEXT_RANGE hours_range = { 0, 2 };
	//layout->SetFontWeight( DWRITE_FONT_WEIGHT_NORMAL, hours_range );
	/*DWRITE_TEXT_RANGE minutes_range = { 3, 2 };
	layout->SetFontSize( 200.0f, minutes_range );*/

	::FLOAT dx = 0.0f;
	switch ( CurrentStage )
	{
	case StartScreen::kSuspending:
		TranslationEasing.SuspendingX += Context->Timer.Elapsed;
		dx = TranslationEasing.SuspendingX.OnFrame(
			);
		Overlay->Context->SetTransform( D2D1::Matrix3x2F(
			D2D1::Matrix3x2F::Translation( { dx, 0 } )
			) );
		Overlay->Context->DrawTextLayout(
			start_point, layout.Get( ),
			TextBrush.Get( )
			);
		Overlay->Context->SetTransform( D2D1::Matrix3x2F(
			D2D1::Matrix3x2F::Identity( )
			) );

		if ( TranslationEasing.ResumingX.Saturated )
		{
			CurrentStage = kSuspended;
		}
		break;
	case StartScreen::kResuming:
		TranslationEasing.ResumingX += Context->Timer.Elapsed;

		dx = TranslationEasing.ResumingX.OnFrame( 
			);
		Overlay->Context->SetTransform( D2D1::Matrix3x2F( 
			D2D1::Matrix3x2F::Translation( { dx, 0 } ) 
			) );
		Overlay->Context->DrawTextLayout( 
			start_point, layout.Get( ),
			TextBrush.Get( ) 
			);
		Overlay->Context->SetTransform( D2D1::Matrix3x2F( 
			D2D1::Matrix3x2F::Identity( ) 
			) );

		if ( TranslationEasing.ResumingX.Saturated )
		{
			CurrentStage = kResumed;

			_Oasis_air_grabot(
				this, OasisAirMsg::kInfo, 
				"switching state to resumed" 
				);
		}

		break;
	case StartScreen::kResumed:
		Overlay->Context->DrawTextLayout( start_point, layout.Get( ), TextBrush.Get( ) );
		// check input ...
		break;
	}
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnUpdateTimeText()
{
	static String::size_type s_bytes = 
		ARRAYSIZE( TimeTextBuffer ) * 
		sizeof String::value_type;

	time(&Timeux);
	::localtime_s( &Time, &Timeux );

	::wcsftime(
		TimeTextBuffer, s_bytes,
		L"%H:%M\n%A %d %B", 
		&Time
		);

	TimeText = TimeTextBuffer;
}

void InteractiveTV::Project::Oasis::Home::StartScreen::OnResized()
{
	CreateWindowSizeDependentResources();
}

void InteractiveTV::Project::Oasis::Home::StartScreen::CreateDeviceIndependentResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "create device-independent resources");

}

void InteractiveTV::Project::Oasis::Home::StartScreen::CreateWindowSizeDependentResources()
{
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;
	FontSizeOrigin = ScreenSize->Height / 8;

	HRESULT result( S_OK );

	if ( !Overlay->DWFactory.Get( ) )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical, "dw factory" );
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
		_Oasis_air_grabot( this, OasisAirMsg::kCritical, "dw text format" );
		return;
	}
	result = TextFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_LEADING );
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical, "dw text alignment" );
		return;
	}
	result = TextFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_FAR );
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical, "dw text para-alignment" );
		return;
	}
	result = TextFormat->SetLineSpacing(
		DWRITE_LINE_SPACING_METHOD_UNIFORM,
		130.0f, 80.0f
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical, "dw text line spacing" );
		return;
	}
	result = Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ),
		TextBrush.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot( this, OasisAirMsg::kCritical, "solid brush" );
		return;
	}

	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "create window-size-dependent resources");
}

void InteractiveTV::Project::Oasis::Home::StartScreen::DiscardDeviceResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "discard device-dependent resources");
}

void InteractiveTV::Project::Oasis::Home::StartScreen::DiscardWindowSizeDependentResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "discard window-size-dependent resources");
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Resume()
{
	_Oasis_air_grabot(this, OasisAirMsg::kWarning, "resume");

	Resumed(this);
}

void InteractiveTV::Project::Oasis::Home::StartScreen::Suspend()
{
	_Oasis_air_grabot(this, OasisAirMsg::kWarning, "suspend");

	Suspended(this);
}

