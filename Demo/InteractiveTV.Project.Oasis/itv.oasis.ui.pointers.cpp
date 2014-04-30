#include "app.precompiled.h"
#include "itv.oasis.ui.pointers.h"
#include "itv.oasis.air.h"

#define _Oasis_ui_pointer "ui/pointers/"


InteractiveTV::Ui::Pointers::Pointers( InteractiveTV::Project::Oasis::Home *home )
	: State( home ), Host( home )
{
	Name = _Oasis_origin _Oasis_ui_pointer;

	Context = &Oasis::GetForCurrentThread( )->Context;
	Overlay = &Context->Engine->Graphics.d2d;
	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;

	Radius = 200.0f;
	CurrentStage = kSuspended;
}

InteractiveTV::Ui::Pointers::~Pointers( )
{
}


void InteractiveTV::Ui::Pointers::Init( )
{

	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );

	Initted( this );
}

void InteractiveTV::Ui::Pointers::Quit( )
{
	Quitted( this );
}

void InteractiveTV::Ui::Pointers::Resume( )
{
	CurrentStage = kResumed;
}

void InteractiveTV::Ui::Pointers::Suspend( )
{
	CurrentStage = kSuspended;
}

void InteractiveTV::Ui::Pointers::OnResized( )
{
}

void InteractiveTV::Ui::Pointers::OnFrameMove( )
{
	if ( CurrentStage == kSuspended ) return; else
	{
		if ( auto ri = Host->BeginRead( ) )
		{
#pragma region Read remote input
			for ( UINT32 i = 0; i < 11; i++ )
			{
				if ( ri->nodes [ 0 ][ i ].body == PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY )
				{

					D2D1_ELLIPSE handmark;
					D2D1::Matrix3x2F translation;
					ISolidColorBrush *border_brush = BorderBrush.Get( );

					if ( ri->nodes [ 0 ][ i ].opennessState ==
						PXCGesture::GeoNode::LABEL_CLOSE )
						border_brush = CloseBorderBrush.Get( );

					translation = D2D1::Matrix3x2F::Translation(
						ScreenSize->Width * (320.0f - ri->nodes [ 0 ][ i ].positionImage.x) / 320.0f,
						ScreenSize->Height * ri->nodes [ 0 ][ i ].positionImage.y / 240.0f
						);

					handmark = D2D1::Ellipse(
						D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
						Radius * 0.5f, Radius * 0.5f
						);

					Overlay->Context->SetTransform( translation );
					Overlay->Context->DrawEllipse( handmark, border_brush, Radius * 0.03f );
					Overlay->Context->FillEllipse( handmark, FillBrush.Get( ) );
					Overlay->Context->SetTransform( D2D1::Matrix3x2F::Identity( ) );

					break;
				}
			}
#pragma endregion

			Host->EndRead( );
		}
	}
}

void InteractiveTV::Ui::Pointers::CreateDeviceResources( )
{
}

void InteractiveTV::Ui::Pointers::CreateDeviceIndependentResources( )
{
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::Orange ),
		CloseBorderBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ),
		BorderBrush.ReleaseAndGetAddressOf( )
		);
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.15, 0.15, 0.15, 0.5 ),
		FillBrush.ReleaseAndGetAddressOf( )
		);
}

void InteractiveTV::Ui::Pointers::CreateWindowSizeDependentResources( )
{
}

void InteractiveTV::Ui::Pointers::DiscardDeviceResources( )
{
}

void InteractiveTV::Ui::Pointers::DiscardWindowSizeDependentResources( )
{
}
