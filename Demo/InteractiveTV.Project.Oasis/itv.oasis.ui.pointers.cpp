#include "app.precompiled.h"
#include "itv.oasis.ui.pointers.h"
#include "itv.oasis.air.h"

#define _Oasis_ui_pointer "ui/pointers/"
#define _Oasis_default_depth_buffer_width_f 320.0f
#define _Oasis_default_depth_buffer_height_f 240.0f


InteractiveTV::Ui::Pointers::DefaultVisuals::DefaultVisuals( InteractiveTV::Ui::Pointers *host )
: IVisuals( host )
{
	Uis[ 0 ] = &Landmark[ 0 ];
	Uis[ 1 ] = &Landmark[ 1 ];
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::Init( )
{
	Landmark[ 0 ].AcquireGlobals( );
	Landmark[ 1 ].AcquireGlobals( );

	Landmark[ 0 ].circle_stroke_width = 0.01f;
	Landmark[ 1 ].circle_stroke_width = 0.01f;
	Landmark[ 0 ].element_dimensions.x = 0.12f;
	Landmark[ 1 ].element_dimensions.x = 0.12f;
	Landmark[ 0 ].element_dimensions.y = 0.12f;
	Landmark[ 1 ].element_dimensions.y = 0.12f;
	Landmark[ 0 ].circle_stroke_opacity = 0.5f;
	Landmark[ 1 ].circle_stroke_opacity = 0.5f;
	Landmark[ 0 ].circle_fill_color = ::D2D1::ColorF( 0.11f, 0.11f, 0.11f, 0.25f );
	Landmark[ 1 ].circle_fill_color = ::D2D1::ColorF( 0.11f, 0.11f, 0.11f, 0.25f );
	Landmark[ 0 ].circle_border_color = ::D2D1::ColorF( 0.15f, 0.15f, 0.15f, 0.5f );
	Landmark[ 1 ].circle_border_color = ::D2D1::ColorF( 0.15f, 0.15f, 0.15f, 0.5f );
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::Quit( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::Resume( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::Suspend( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::OnResized( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::OnFrameMove( )
{
	if ( Host->IsVisible[ 0 ] )
	{
		if ( Host->IsDragging[ 0 ] ) Landmark[ 0 ].circle_border_brush->SetColor(
			::D2D1::ColorF( 0.25f, 0.25f, 0.25f, 0.5f )
			);
		else Landmark[ 0 ].circle_border_brush->SetColor(
			::D2D1::ColorF( 0.15f, 0.15f, 0.15f, 0.5f )
			);
		Landmark[ 0 ].element_position_ts = Host->LastAdjustedRelativePosition[ 0 ];
		Landmark[ 0 ].DrawFillCircle( );
	}
	if ( Host->IsVisible[ 1 ] )
	{
		if ( Host->IsDragging[ 1 ] ) Landmark[ 1 ].circle_border_brush->SetColor(
			::D2D1::ColorF( 0.25f, 0.25f, 0.25f, 0.5f )
			);
		else Landmark[ 1 ].circle_border_brush->SetColor(
			::D2D1::ColorF( 0.15f, 0.15f, 0.15f, 0.5f )
			);
		Landmark[ 1 ].element_position_ts = Host->LastAdjustedRelativePosition[ 1 ];
		Landmark[ 1 ].DrawFillCircle( );
	}
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::CreateDeviceResources( )
{
	Landmark[ 0 ].CreateDeviceResources( );
	Landmark[ 1 ].CreateDeviceResources( );
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::CreateDeviceIndependentResources( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::CreateWindowSizeDependentResources( )
{
}


void InteractiveTV::Ui::Pointers::DefaultVisuals::DiscardDeviceResources( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::DiscardWindowSizeDependentResources( )
{
}

void InteractiveTV::Ui::Pointers::DefaultVisuals::OnGestureReceived(
	Remote::Input::GestureAppMessageArg
	)
{
}

InteractiveTV::Ui::Pointers::Pointers( InteractiveTV::Oasis::Home *home )
: State( home ), Host( home ), DefaultVisuals( this )
{
	Name = _Oasis_origin _Oasis_ui_pointer;

	Visuals = &DefaultVisuals;
	Context = &Oasis::GetForCurrentThread( )->Context;
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;

	IsActive[ 0 ] = FALSE;
	IsActive[ 1 ] = FALSE;
	IsVisible[ 0 ] = TRUE;
	IsVisible[ 1 ] = FALSE;
	IsDragging[ 0 ] = FALSE;
	IsDragging[ 1 ] = FALSE;
	IsHoverable[ 0 ] = TRUE;
	IsHoverable[ 1 ] = FALSE;
	LastActualPosition[ 0 ].x = 0.0f;
	LastActualPosition[ 1 ].y = 0.0f;
	LastRelativePosition[ 0 ].x = 0.0f;
	LastRelativePosition[ 1 ].y = 0.0f;
	LastAdjustedRelativePosition[ 0 ].x = 0.0f;
	LastAdjustedRelativePosition[ 1 ].y = 0.0f;
	DepthBufferDimensions.x = _Oasis_default_depth_buffer_width_f;
	DepthBufferDimensions.y = _Oasis_default_depth_buffer_height_f;

	CurrentStage = kSuspended;
}


InteractiveTV::Ui::Pointers::~Pointers( )
{
}

void InteractiveTV::Ui::Pointers::Init( )
{
	Visuals->Init( );
	Initted( this );
}

void InteractiveTV::Ui::Pointers::Quit( )
{
	Visuals->Quit( );
	Quitted( this );
}

void InteractiveTV::Ui::Pointers::Resume( )
{
	Visuals->Resume( );
	CurrentStage = kResumed;
}

void InteractiveTV::Ui::Pointers::Suspend( )
{
	Visuals->Suspend( );
	CurrentStage = kSuspended;
}

void InteractiveTV::Ui::Pointers::OnResized( )
{
	Visuals->OnResized( );
}

void InteractiveTV::Ui::Pointers::OnFrameMove( )
{
	IsActive[ 0 ] = FALSE;
	IsActive[ 1 ] = FALSE;
	IsDragging[ 0 ] = FALSE;
	IsDragging[ 1 ] = FALSE;

	if ( CurrentStage == kSuspended ) return; else
	{
		if ( auto ri = Host->BeginRead( ) )
		{
#pragma region Read remote input

			for ( UINT32 hand_index = 0; hand_index < HandsToProcess; hand_index++ )
			for ( UINT32 node_index = 0; node_index < NodesToProcess; node_index++ )
			if ( ri->Nodes[ hand_index ][ node_index ].body ==
				PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY )
			{
				IsActive[ hand_index ] = TRUE;

				if ( ri->Nodes[ hand_index ][ node_index ].opennessState == PXCGesture::GeoNode::LABEL_CLOSE )
					IsDragging[ hand_index ] = TRUE;
				else
					IsDragging[ hand_index ] = FALSE;

				if ( IsHoverable[ hand_index ] )
				{
					::Nena::Vector2 depth_buffer_position;
					depth_buffer_position.x = (DepthBufferDimensions.x - ri->Nodes[ hand_index ][ node_index ].positionImage.x);
					depth_buffer_position.y = ri->Nodes[ hand_index ][ node_index ].positionImage.y;
					LastActualPosition[ hand_index ].x = ScreenSize->Width * depth_buffer_position.x / DepthBufferDimensions.x;
					LastActualPosition[ hand_index ].y = ScreenSize->Height * depth_buffer_position.y / DepthBufferDimensions.y;
					LastRelativePosition[ hand_index ].x = LastActualPosition[ hand_index ].x / ScreenSize->Width;
					LastRelativePosition[ hand_index ].y = LastActualPosition[ hand_index ].y / ScreenSize->Height;
					LastRelativePosition[ hand_index ].x -= 0.5f; LastRelativePosition[ hand_index ].x *= 2.0f;
					LastRelativePosition[ hand_index ].y -= 0.5f; LastRelativePosition[ hand_index ].y *= 2.0f;
					LastAdjustedRelativePosition[ hand_index ] = LastRelativePosition[ hand_index ] / 0.6f;
				}

				break;
			}
#pragma endregion
			Host->EndRead( );
		}
	}

	Visuals->OnFrameMove( );
}

void InteractiveTV::Ui::Pointers::CreateDeviceResources( )
{
	Visuals->CreateDeviceResources( );
}

void InteractiveTV::Ui::Pointers::CreateDeviceIndependentResources( )
{
	Visuals->CreateDeviceIndependentResources( );
}

void InteractiveTV::Ui::Pointers::CreateWindowSizeDependentResources( )
{
	Visuals->CreateWindowSizeDependentResources( );
}

void InteractiveTV::Ui::Pointers::DiscardDeviceResources( )
{
	Visuals->DiscardDeviceResources( );
}

void InteractiveTV::Ui::Pointers::DiscardWindowSizeDependentResources( )
{
	Visuals->DiscardWindowSizeDependentResources( );
}
