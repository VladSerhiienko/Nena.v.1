#include "app.precompiled.h"
#include "itv.oasis.ui.pointers.h"
#include "itv.oasis.ui.dialog.h"

#define _Oasis_ui_dialog "ui/dialog/"


InteractiveTV::Ui::Dialog::Dialog( InteractiveTV::Oasis::Home *home )
: InteractiveTV::Oasis::Home::State( home )
{
	Radius = 200 * 3.5;
	Name = _Oasis_origin _Oasis_ui_dialog;

	Context = &Oasis::GetForCurrentThread( )->Context;
	Overlay = &Context->Engine->Graphics.d2d;
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;

	CurrentStage = Dialog::kSuspended;
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
	button->Index = Buttons.size( );
	button->IconPath = icon;

	if ( button->Index == 0 )
		button->Radius = 200.0f;

	Buttons.push_back( button );
	for ( auto &b : Buttons )
		b->PanelSize = Buttons.size( ) - 1;
}

void InteractiveTV::Ui::Dialog::Init( )
{
	auto total = 0.5f;
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
	Easing.SuspendingRadius.Reset( );
	Easing.ResumingRadius.Reset( );
	CurrentStage = kResuming;
	for ( auto &b : Buttons )
		b->Resume( );
}

void InteractiveTV::Ui::Dialog::Suspend( )
{
	Easing.SuspendingRadius.Reset( );
	Easing.ResumingRadius.Reset( );
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
		break;
	case Dialog::kResuming:
		Easing.ResumingRadius += Context->Timer.Elapsed;
		Radius = Easing.ResumingRadius.OnFrame( );
		break;
	}

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
	{
		b->OnFrameMove( );
		if ( b->CurrentSelectionStage == b->kSelected )
		{
			ButtonSelected( this, b );
			Suspend( );
		}
	}

	switch ( CurrentStage )
	{
	case Dialog::kSuspending:
		if ( Easing.SuspendingRadius.Saturated )
			Easing.SuspendingRadius.Reset( ),
			Easing.ResumingRadius.Reset( ),
			CurrentStage = kSuspended;
		break;
	case Dialog::kResuming:
		if ( Easing.ResumingRadius.Saturated )
			Easing.SuspendingRadius.Reset( ),
			Easing.ResumingRadius.Reset( ),
			CurrentStage = kResumed;
		break;
	}
}

void InteractiveTV::Ui::Dialog::CreateDeviceResources( )
{
	if ( !Overlay->Context.Get( ) ) return;

	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.15f, 0.15f, 0.15f ),
		BorderBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.12f, 0.12f, 0.12f ),
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