#include "app.precompiled.h"
#include "itv.oasis.ui.button.h"
#include "itv.oasis.ui.pointers.h"

#define _Oasis_ui_button "ui/button/"
#define _Oasis_assets "./Assets/"
#define _Oasis_lock_time "Lock-Time.png"
#define _Oasis_lock_open "Lock-Open.png"
#define _Oasis_lock_closed "Lock.png"
#define _Oasis_media_pause "Media-Pause.png"

InteractiveTV::Ui::Button::Button( InteractiveTV::Project::Oasis::Home *home )
: State( nullptr ), Host( home )
{
	Position = 1;
	PanelSize = 3;
	Radius = 300.0f;
	CurrentStage = kSuspended;
	CurrentSelectionStage = kDeselected;

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

	if ( Position == 0 ) Offset = 0.0f, AngularOffset = 360.0f;
	else Offset = Radius * 0.7, AngularOffset = 360.0f * (FLOAT) (Position - 1) / (FLOAT) PanelSize + 180.0f;

	if ( PanelSize == 2 ) AngularOffset *= -1, AngularOffset += 90.0f;
	else AngularOffset *= -1;

	auto total = 2.0f;

	Easing.ResumingOffset.Params.Total = total;
	Easing.ResumingOffset.Params.Elapsed = 0.0f;
	Easing.ResumingOffset.Params.Offset = Offset * 0.3f;
	Easing.ResumingOffset.Params.Distance = Offset * 0.7f;

	Easing.SuspendingOffset.Params.Total = total;
	Easing.SuspendingOffset.Params.Elapsed = 0.0f;
	Easing.SuspendingOffset.Params.Offset = Offset;
	Easing.SuspendingOffset.Params.Distance = Offset * -0.7f;

	Easing.ResumingRadius.Params.Total = total;
	Easing.ResumingRadius.Params.Elapsed = 0.0f;
	Easing.ResumingRadius.Params.Offset = Radius * 0.2f;
	Easing.ResumingRadius.Params.Distance = Radius * 0.8f;

	Easing.SuspendingRadius.Params.Total = total;
	Easing.SuspendingRadius.Params.Elapsed = 0.0f;
	Easing.SuspendingRadius.Params.Offset = Radius;
	Easing.SuspendingRadius.Params.Distance = Radius * -0.8f;

	Easing.ResumingAnglularOffset.Params.Total = total;
	Easing.ResumingAnglularOffset.Params.Elapsed = 0.0f;
	Easing.ResumingAnglularOffset.Params.Offset = AngularOffset * 0.5f;
	Easing.ResumingAnglularOffset.Params.Distance = AngularOffset * 0.5f;

	Easing.SuspendingAngularOffset.Params.Total = total;
	Easing.SuspendingAngularOffset.Params.Elapsed = 0.0f;
	Easing.SuspendingAngularOffset.Params.Offset = AngularOffset;
	Easing.SuspendingAngularOffset.Params.Distance = AngularOffset * -0.5f;

	Easing.ProgressArcProgressSelecting.Params.Total = total;
	Easing.ProgressArcProgressSelecting.Params.Elapsed = 0.0f;
	Easing.ProgressArcProgressSelecting.Params.Offset = 0.0f;
	Easing.ProgressArcProgressSelecting.Params.Distance = 360.0f;

	Easing.ProgressArcProgressDeselecting.Params.Total = total;
	Easing.ProgressArcProgressDeselecting.Params.Elapsed = 0.0f;
	Easing.ProgressArcProgressDeselecting.Params.Offset = 360.0f;
	Easing.ProgressArcProgressDeselecting.Params.Distance = -360.0f;

	/*if ( Position == 0 )
	{
		Easing.ResumingAnglularOffset.Params.Total = 0.0f;
		Easing.ResumingAnglularOffset.Params.Total = 0.0f;
	}*/

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
	CurrentStage = kResuming;
}
void InteractiveTV::Ui::Button::Suspend( )
{
	CurrentStage = kSuspending;
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

		Easing.ResumingAnglularOffset += Context->Timer.Elapsed;
		Easing.ResumingRadius += Context->Timer.Elapsed;
		Easing.ResumingOffset += Context->Timer.Elapsed;

		AngularOffset = Easing.ResumingAnglularOffset.OnFrame( );
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

	Border = D2D1::Ellipse(
		D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
		Radius * 0.35f, Radius * 0.35f
		);

	Overlay->Context->SetTransform( xform );
	DrawProgressArc( );
	Overlay->Context->DrawEllipse( Border, BorderBrush.Get( ), Radius * 0.03f );
	Overlay->Context->FillEllipse( Border, FillBrush.Get( ) );
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
	D2D1::Matrix3x2F
		translate_progress,
		rotate_progress,
		xform_progress;

	Microsoft::WRL::ComPtr<ID2D1PathGeometry> progress_geom;
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> progress_sink;

	Overlay->D2DFactory->CreatePathGeometry( progress_geom.ReleaseAndGetAddressOf( ) );
	progress_geom->Open( progress_sink.ReleaseAndGetAddressOf( ) );
	progress_sink->SetFillMode( D2D1_FILL_MODE_WINDING );

	progress_sink->BeginFigure(
		D2D1::Point2F( Border.point.x - Radius * 0.35f, Border.point.y ),
		D2D1_FIGURE_BEGIN_HOLLOW
		);

	float progress = 355.0f;
	auto arc_sz = progress > 180.0f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
	//auto progress_brush = DeselectionProgressBrush.Get( );
	auto progress_brush = SelectionProgressBrush.Get( );

	D2D1_POINT_2F progress_end = D2D1::Point2F( );
	translate_progress = D2D1::Matrix3x2F::Translation( -Radius * 0.35f, 0 );
	rotate_progress = D2D1::Matrix3x2F::Rotation( progress );
	xform_progress = translate_progress * rotate_progress;
	progress_end = xform_progress.TransformPoint( progress_end );

	progress_end.x += Border.point.x;
	progress_end.y += Border.point.y;

	ProgressArc = D2D1::ArcSegment(
		D2D1::Point2F( progress_end.x, progress_end.y ),
		D2D1::SizeF( Border.radiusX, Border.radiusY ),
		360.0f,
		D2D1_SWEEP_DIRECTION_CLOCKWISE,
		arc_sz
		);

	progress_sink->AddArc( ProgressArc );
	progress_sink->EndFigure( D2D1_FIGURE_END_OPEN );
	progress_sink->Close( );

	Overlay->Context->DrawGeometry( progress_geom.Get( ), progress_brush, Radius * 0.06f );
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
		D2D1::ColorF( D2D1::ColorF::Green ),
		DeselectionProgressBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::Orange ),
		SelectionProgressBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush( 
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ), 
		BorderBrush.ReleaseAndGetAddressOf( ) 
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.15, 0.15, 0.15 ),
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
