#include "app.precompiled.h"
#include "itv.oasis.ui.button.h"
#include "itv.oasis.ui.pointers.h"

#define _Oasis_ui_button "ui/button/"
#define _Oasis_assets "./Assets/"
#define _Oasis_lock_time "Lock-Time.png"
#define _Oasis_lock_open "Lock-Open.png"
#define _Oasis_lock_closed "Lock.png"
#define _Oasis_media_pause "Media-Pause.png"

InteractiveTV::Ui::Button::Button( InteractiveTV::Oasis::Home *home )
: State( nullptr ), Host( home )
{
	Index = 1;
	PanelSize = 3;
	Radius = 300.0f;
	Progress = 0.0f;
	CurrentStage = kSuspended;
	CurrentSelectionStage = kDeselected;
	CurrentPosition.x = 0.0f;
	CurrentPosition.y = 0.0f;

	Name = _Oasis_origin _Oasis_ui_button;
	IconPath = _Oasis_assets _Oasis_media_pause;

	Context = &Oasis::GetForCurrentThread( )->Context;
	Overlay = &Context->Engine->Graphics.d2d;
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;
}

InteractiveTV::Ui::Button::~Button( )
{
}

void InteractiveTV::Ui::Button::Init( )
{

	if ( Index == 0 ) Offset = 0.0f, AngularOffset = 360.0f;
	else Offset = Radius * 0.7f, AngularOffset = 360.0f * (FLOAT) (Index - 1) / (FLOAT) PanelSize + 180.0f;

	if ( PanelSize == 2 ) AngularOffset *= -1.f, AngularOffset += 90.0f;
	else AngularOffset *= -1.f;

	auto total = 0.5f;

	Easing.ResumingOffset.Params.Total = total;
	Easing.ResumingOffset.Params.Elapsed = 0.0f;
	Easing.ResumingOffset.Params.Offset = Offset * 0.2f;
	Easing.ResumingOffset.Params.Distance = Offset * 0.8f;

	Easing.SuspendingOffset.Params.Total = total;
	Easing.SuspendingOffset.Params.Elapsed = 0.0f;
	Easing.SuspendingOffset.Params.Offset = Offset;
	Easing.SuspendingOffset.Params.Distance = Offset * -0.8f;

	Easing.ResumingRadius.Params.Total = total;
	Easing.ResumingRadius.Params.Elapsed = 0.0f;
	Easing.ResumingRadius.Params.Offset = Radius * 0.2f;
	Easing.ResumingRadius.Params.Distance = Radius * 0.8f;

	Easing.SuspendingRadius.Params.Total = total;
	Easing.SuspendingRadius.Params.Elapsed = 0.0f;
	Easing.SuspendingRadius.Params.Offset = Radius;
	Easing.SuspendingRadius.Params.Distance = Radius * -0.8f;

	Easing.ResumingAngularOffset.Params.Total = total;
	Easing.ResumingAngularOffset.Params.Elapsed = 0.0f;
	Easing.ResumingAngularOffset.Params.Offset = AngularOffset * 0.5f;
	Easing.ResumingAngularOffset.Params.Distance = AngularOffset * 0.5f;

	Easing.SuspendingAngularOffset.Params.Total = total;
	Easing.SuspendingAngularOffset.Params.Elapsed = 0.0f;
	Easing.SuspendingAngularOffset.Params.Offset = AngularOffset;
	Easing.SuspendingAngularOffset.Params.Distance = AngularOffset * -0.5f;

	Easing.ProgressArcSelecting.Params.Total = 1.0f;
	Easing.ProgressArcSelecting.Params.Elapsed = 0.0f;
	Easing.ProgressArcSelecting.Params.Offset = 0.0f;
	Easing.ProgressArcSelecting.Params.Distance = 360.0f;

	Easing.ProgressArcDeselecting.Params.Total = 1.0f;
	Easing.ProgressArcDeselecting.Params.Elapsed = 0.0f;
	Easing.ProgressArcDeselecting.Params.Offset = 360.0f;
	Easing.ProgressArcDeselecting.Params.Distance = -360.0f;

	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );

	Initted( this );
}

void InteractiveTV::Ui::Button::Quit( )
{
	Quitted( this );
}

void InteractiveTV::Ui::Button::Resume( )
{
	Easing.ResumingAngularOffset.Reset( );
	Easing.ResumingOffset.Reset( );
	Easing.ResumingRadius.Reset( );
	Easing.SuspendingAngularOffset.Reset( );
	Easing.SuspendingOffset.Reset( );
	Easing.SuspendingRadius.Reset( );
	Easing.ProgressArcDeselecting.Reset( );
	Easing.ProgressArcSelecting.Reset( );

	CurrentSelectionStage = Button::kDeselected;
	Pointers->IsVisible = TRUE;
	CurrentStage = kResuming;
}

void InteractiveTV::Ui::Button::Suspend( )
{
	Easing.ResumingAngularOffset.Reset( );
	Easing.ResumingOffset.Reset( );
	Easing.ResumingRadius.Reset( );
	Easing.SuspendingAngularOffset.Reset( );
	Easing.SuspendingOffset.Reset( );
	Easing.SuspendingRadius.Reset( );
	Easing.ProgressArcDeselecting.Reset( );
	Easing.ProgressArcSelecting.Reset( );

	Pointers->IsVisible = TRUE;
	CurrentStage = kSuspending;
}

void InteractiveTV::Ui::Button::Select( )
{
	Easing.ProgressArcDeselecting.Reset( );
	CurrentSelectionStage = kSelecting;
}

void InteractiveTV::Ui::Button::Deselect( )
{
	Easing.ProgressArcSelecting.Reset( );
	CurrentSelectionStage = kDeselecting;
}

void InteractiveTV::Ui::Button::OnResized( )
{
}

void InteractiveTV::Ui::Button::OnFrameMove( )
{
	if ( CurrentStage == kSuspended )
		return;

	Rect icon_rect;
	D2D1::Matrix3x2F
		translate_scene,
		rotate_back,
		translate,
		rotate,
		xform;

	switch ( CurrentStage )
	{
	case InteractiveTV::Ui::Button::kSuspending:

		Easing.SuspendingAngularOffset += Context->Timer.Elapsed;
		Easing.SuspendingRadius += Context->Timer.Elapsed;
		Easing.SuspendingOffset += Context->Timer.Elapsed;

		AngularOffset = Easing.SuspendingAngularOffset.OnFrame( );
		Radius = Easing.SuspendingRadius.OnFrame( );
		Offset = Easing.SuspendingOffset.OnFrame( );

		break;
	case InteractiveTV::Ui::Button::kResuming:

		Easing.ResumingAngularOffset += Context->Timer.Elapsed;
		Easing.ResumingRadius += Context->Timer.Elapsed;
		Easing.ResumingOffset += Context->Timer.Elapsed;

		AngularOffset = Easing.ResumingAngularOffset.OnFrame( );
		Radius = Easing.ResumingRadius.OnFrame( );
		Offset = Easing.ResumingOffset.OnFrame( );

		break;
	}

	translate_scene = D2D1::Matrix3x2F::Translation(
	{
		ScreenSize->Width * 0.5f - Radius * 0.5f,
		ScreenSize->Height * 0.5f - Radius* 0.5f
	} );

	translate = D2D1::Matrix3x2F::Translation( { 0, Offset } );
	rotate_back = D2D1::Matrix3x2F::Rotation( -AngularOffset );
	rotate = D2D1::Matrix3x2F::Rotation( AngularOffset );

	icon_rect = D2D1::RectF( 0, 0, Radius, Radius );
	xform = rotate_back * translate * rotate * translate_scene;

	CurrentPosition.x = xform._31;
	CurrentPosition.y = xform._32;

	if ( CurrentStage == kResumed &&
		Pointers && Pointers->IsDragging )
	{
		auto distance2 = ::Nena::Vector2::DistanceSquared(
			Pointers->LastPosition, CurrentPosition
			);
		if ( distance2 <= (Radius * Radius * 0.2f) &&
			CurrentSelectionStage != kSelected )
		{
			if ( CurrentSelectionStage != kSelected )
			{
				Easing.ProgressArcDeselecting.Params.Elapsed =
					Easing.ProgressArcSelecting.Params.Total -
					Easing.ProgressArcSelecting.Params.Elapsed;

				CurrentSelectionStage = kSelecting;
				Pointers->IsVisible = FALSE;
				Pointers->IsMovable = FALSE;
			}
			else if ( CurrentSelectionStage != kDeselected )
			{
				Easing.ProgressArcSelecting.Params.Elapsed =
					Easing.ProgressArcDeselecting.Params.Total -
					Easing.ProgressArcDeselecting.Params.Elapsed;

				CurrentSelectionStage = kDeselecting;
				Pointers->IsVisible = TRUE;
				Pointers->IsMovable = TRUE;
			}
		}
	}
	else
	{
		if ( CurrentSelectionStage != kDeselected )
		{
			Easing.ProgressArcSelecting.Params.Elapsed =
				Easing.ProgressArcDeselecting.Params.Total -
				Easing.ProgressArcDeselecting.Params.Elapsed;

			CurrentSelectionStage = kDeselecting;
			Pointers->IsVisible = TRUE;
			Pointers->IsMovable = TRUE;
		}
	}

	Border = D2D1::Ellipse(
		D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
		Radius * 0.35f, Radius * 0.35f
		);

	auto fill_brush = CurrentSelectionStage == kSelected
		? SelectedFillBrush.Get( )
		: FillBrush.Get( );

	Overlay->Context->SetTransform( xform );
	DrawProgressArc( );
	Overlay->Context->DrawEllipse( Border, BorderBrush.Get( ), Radius * 0.03f );
	Overlay->Context->FillEllipse( Border, fill_brush );
	Overlay->Context->DrawBitmap( Icon.Get( ), &icon_rect );
	Overlay->Context->SetTransform( D2D1::Matrix3x2F::Identity( ) );

	switch ( CurrentStage )
	{
	case InteractiveTV::Ui::Button::kSuspending:
		if ( Easing.SuspendingRadius.Saturated )
			CurrentStage = kSuspended;
		break;
	case InteractiveTV::Ui::Button::kResuming:
		if ( Easing.ResumingRadius.Saturated )
			CurrentStage = kResumed;
		break;
	}
}

void InteractiveTV::Ui::Button::DrawProgressArc( )
{
	if ( CurrentSelectionStage == kDeselected ||
		CurrentSelectionStage == kSelected)
		return;

	auto progress_brush = SelectionProgressBrush.Get( );

	switch ( CurrentSelectionStage )
	{
	case Button::kDeselecting:
		Easing.ProgressArcDeselecting += Context->Timer.Elapsed;
		Progress = Easing.ProgressArcDeselecting.OnFrame( );
		progress_brush = DeselectionProgressBrush.Get( );
		break;
	case Button::kSelecting:
		Easing.ProgressArcSelecting += Context->Timer.Elapsed;
		Progress = Easing.ProgressArcSelecting.OnFrame( );
		progress_brush = SelectionProgressBrush.Get( );
		break;
	}

	if ( Progress >= 360.0f )
		Progress = 359.9f;

	D2D1::Matrix3x2F
		translate_progress,
		rotate_progress,
		xform_progress;

	Microsoft::WRL::ComPtr<ID2D1PathGeometry> progress_geom;
	if ( FAILED( Overlay->D2DFactory->CreatePathGeometry(
		progress_geom.ReleaseAndGetAddressOf( )
		) ) ) return;
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> progress_sink;
	if ( FAILED( progress_geom->Open(
		progress_sink.ReleaseAndGetAddressOf( )
		) ) ) return;
	auto arc_sz = Progress > 180.0f
		? D2D1_ARC_SIZE_LARGE
		: D2D1_ARC_SIZE_SMALL;

	progress_sink->SetFillMode( 
		D2D1_FILL_MODE_WINDING 
		);
	progress_sink->BeginFigure(
		D2D1::Point2F( Border.point.x - Radius * 0.35f, Border.point.y ),
		D2D1_FIGURE_BEGIN_HOLLOW
		);

	translate_progress = D2D1::Matrix3x2F::Translation( -Radius * 0.35f, 0 );
	rotate_progress = D2D1::Matrix3x2F::Rotation( Progress );
	xform_progress = translate_progress * rotate_progress;

	D2D1_POINT_2F progress_end = D2D1::Point2F( );
	progress_end = xform_progress.TransformPoint( progress_end );
	progress_end.x += Border.point.x;
	progress_end.y += Border.point.y;

	ProgressArc = D2D1::ArcSegment(
		D2D1::Point2F( progress_end.x, progress_end.y ),
		D2D1::SizeF( Border.radiusX, Border.radiusY ),
		360.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE,
		arc_sz
		);

	progress_sink->AddArc( 
		ProgressArc
		);
	progress_sink->EndFigure(
		D2D1_FIGURE_END_OPEN
		);
	progress_sink->Close( 
		);
	progress_sink.ReleaseAndGetAddressOf( 
		);

	Overlay->Context->DrawGeometry( 
		progress_geom.Get( ), 
		progress_brush, 
		Radius * 0.1f 
		);

	switch ( CurrentSelectionStage )
	{
	case Button::kDeselecting:
		if ( Easing.ProgressArcDeselecting.Saturated )
			Easing.ResumingAngularOffset.Reset( ),
			Easing.ResumingOffset.Reset( ),
			Easing.ResumingRadius.Reset( ),
			Easing.SuspendingAngularOffset.Reset( ),
			Easing.SuspendingOffset.Reset( ),
			Easing.SuspendingRadius.Reset( ),
			Easing.ProgressArcDeselecting.Reset( ),
			Easing.ProgressArcSelecting.Reset( ),
			CurrentSelectionStage = kDeselected;
		break;
	case Button::kSelecting:
		if ( Easing.ProgressArcSelecting.Saturated )
			Easing.ResumingAngularOffset.Reset( ),
			Easing.ResumingOffset.Reset( ),
			Easing.ResumingRadius.Reset( ),
			Easing.SuspendingAngularOffset.Reset( ),
			Easing.SuspendingOffset.Reset( ),
			Easing.SuspendingRadius.Reset( ),
			Easing.ProgressArcDeselecting.Reset( ),
			Easing.ProgressArcSelecting.Reset( ),
			CurrentSelectionStage = kSelected;
		break;
	}
}

void InteractiveTV::Ui::Button::CreateDeviceResources( )
{
	using namespace Nena::Graphics::Resources;

	Icon = BitmapHelp::LoadFromFile( 
		IconPath 
		);
	LockTime = BitmapHelp::LoadFromFile( 
		_Oasis_assets _Oasis_lock_time 
		);
	LockOpen = BitmapHelp::LoadFromFile( 
		_Oasis_assets _Oasis_lock_open 
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::LightSeaGreen ),
		DeselectionProgressBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::LightPink ),
		SelectionProgressBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush( 
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ), 
		BorderBrush.ReleaseAndGetAddressOf( ) 
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.05f, 0.05f, 0.05f ),
		SelectedFillBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.15f, 0.15f, 0.15f ),
		FillBrush.ReleaseAndGetAddressOf( )
		);
}

void InteractiveTV::Ui::Button::CreateDeviceIndependentResources( )
{
}

void InteractiveTV::Ui::Button::CreateWindowSizeDependentResources( )
{
}

void InteractiveTV::Ui::Button::DiscardDeviceResources( )
{
}

void InteractiveTV::Ui::Button::DiscardWindowSizeDependentResources( )
{
}
