#include "app.precompiled.h"
#include "itv.oasis.ui.pointers.h"
#include "itv.oasis.ui.dialog.h"

#define _Oasis_ui_dialog "ui/dialog/"


InteractiveTV::Ui::Dialog::Dialog( InteractiveTV::Project::Oasis::Home *home )
	: InteractiveTV::Project::Oasis::Home::State(home )
{
	Radius = 200 * 3.5;
	Name = _Oasis_origin _Oasis_ui_dialog;

	Context = &Oasis::GetForCurrentThread( )->Context;
	Overlay = &Context->Engine->Graphics.d2d;
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;
}

InteractiveTV::Ui::Dialog::~Dialog( )
{
	for ( auto &b : Buttons )
		delete b, b = NULL;
	Buttons.clear( );
}

void InteractiveTV::Ui::Dialog::Add( String icon )
{
	auto button = new Ui::Button( Host );
	button->Position = Buttons.size( );
	button->IconPath = icon;

	if ( button->Position == 0 )
		button->Radius = 200.0f;

	Buttons.push_back( button );
	for ( auto &b : Buttons )
		b->PanelSize = Buttons.size( ) - 1;
}

void InteractiveTV::Ui::Dialog::Init( )
{
	auto total = 1.0f;
	Easing.ResumingRadius.Params.Total = total;
	Easing.ResumingRadius.Params.Elapsed = 0.0f;
	Easing.ResumingRadius.Params.Offset = Radius * 0.2f;
	Easing.ResumingRadius.Params.Distance = Radius * 0.8f;

	Easing.SuspendingRadius.Params.Total = total;
	Easing.SuspendingRadius.Params.Elapsed = 0.0f;
	Easing.SuspendingRadius.Params.Offset = Radius;
	Easing.SuspendingRadius.Params.Distance = Radius * -0.8f;

	for ( auto &b : Buttons )
		b->Init( );
}

void InteractiveTV::Ui::Dialog::Quit( )
{
	for ( auto &b : Buttons )
		b->Quit( );
}

void InteractiveTV::Ui::Dialog::Resume( )
{
	CurrentStage = kResuming;
	for ( auto &b : Buttons )
		b->Resume( );
}

void InteractiveTV::Ui::Dialog::Suspend( )
{
	CurrentStage = kSuspending;
	for ( auto &b : Buttons )
		b->Suspend( );
}

void InteractiveTV::Ui::Dialog::Set( 
	Ui::Pointers *instance
	)
{
	for ( auto &b : Buttons )
		b->Pointers = instance;
}


void InteractiveTV::Ui::Dialog::OnResized( )
{
}

void InteractiveTV::Ui::Dialog::OnFrameMove( )
{
	if ( CurrentStage == kSuspended )
		return; 
	
	D2D1::Matrix3x2F
		translate_scene,
		translate,
		xform;

	switch ( CurrentStage )
	{
	case Dialog::kSuspending:

		Easing.SuspendingRadius += Context->Timer.Elapsed;
		Radius = Easing.SuspendingRadius.OnFrame( );

		translate_scene = D2D1::Matrix3x2F::Translation(
		{
			ScreenSize->Width * 0.5f - Radius * 0.5f,
			ScreenSize->Height * 0.5f - Radius* 0.5f
		} );

		xform = translate_scene;

		Panel = D2D1::Ellipse(
			D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
			Radius * 0.5f, Radius * 0.5f
			);

		Overlay->Context->SetTransform( xform );
		Overlay->Context->DrawEllipse( Panel, BorderBrush.Get( ), Radius * 0.03f );
		Overlay->Context->FillEllipse( Panel, FillBrush.Get( ) );
		Overlay->Context->SetTransform( D2D1::Matrix3x2F::Identity( ) );

		for ( auto &b : Buttons )
			b->OnFrameMove( );

		if ( Easing.SuspendingRadius.Saturated )
			CurrentStage = kSuspended;

		break;
	case Dialog::kResuming:

		Easing.ResumingRadius += Context->Timer.Elapsed;
		Radius = Easing.ResumingRadius.OnFrame( );

		translate_scene = D2D1::Matrix3x2F::Translation(
		{
			ScreenSize->Width * 0.5f - Radius * 0.5f,
			ScreenSize->Height * 0.5f - Radius* 0.5f
		} );

		xform = translate_scene;

		Panel = D2D1::Ellipse(
			D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
			Radius * 0.5f, Radius * 0.5f
			);

		Overlay->Context->SetTransform( xform );
		Overlay->Context->DrawEllipse( Panel, BorderBrush.Get( ), Radius * 0.03f );
		Overlay->Context->FillEllipse( Panel, FillBrush.Get( ) );
		Overlay->Context->SetTransform( D2D1::Matrix3x2F::Identity( ) );

		for ( auto &b : Buttons )
			b->OnFrameMove( );

		if ( Easing.ResumingRadius.Saturated )
			CurrentStage = kResumed;
		break;
	case Dialog::kResumed:

		translate_scene = D2D1::Matrix3x2F::Translation(
		{
			ScreenSize->Width * 0.5f - Radius * 0.5f,
			ScreenSize->Height * 0.5f - Radius* 0.5f
		} );

		xform = translate_scene;

		Panel = D2D1::Ellipse(
			D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
			Radius * 0.5f, Radius * 0.5f
			);

		Overlay->Context->SetTransform( xform );
		Overlay->Context->DrawEllipse( Panel, BorderBrush.Get( ), Radius * 0.03f );
		Overlay->Context->FillEllipse( Panel, FillBrush.Get( ) );
		Overlay->Context->SetTransform( D2D1::Matrix3x2F::Identity( ) );

		for ( auto &b : Buttons )
			b->OnFrameMove( );

		break;
	}
}

void InteractiveTV::Ui::Dialog::CreateDeviceResources( )
{
	using namespace Nena::Graphics::Resources;

	Overlay->Context->CreateSolidColorBrush( 
		D2D1::ColorF( 0.15, 0.15, 0.15 ),
		BorderBrush.ReleaseAndGetAddressOf( ) 
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.12, 0.12, 0.12 ),
		FillBrush.ReleaseAndGetAddressOf( )
		);
}

void InteractiveTV::Ui::Dialog::CreateDeviceIndependentResources( )
{
}

void InteractiveTV::Ui::Dialog::CreateWindowSizeDependentResources( )
{
}

void InteractiveTV::Ui::Dialog::DiscardDeviceResources( )
{
}

void InteractiveTV::Ui::Dialog::DiscardWindowSizeDependentResources( )
{
}