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

	Element = NULL;
	IsActive = FALSE;
	IsVisible = TRUE;
	IsMovable = TRUE;
	IsDragging = FALSE;
	LastPosition.x = 0;
	LastPosition.y = 0;

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
	IsActive = FALSE;
	IsDragging = FALSE;
	if ( CurrentStage == kSuspended ) return; else
	{
		if ( auto ri = Host->BeginRead( ) )
		{
#pragma region Read remote input

			for ( UINT32 i = 0; i < 11; i++ )
			{
				if ( ri->Nodes[ 0 ][ i ].body == PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY )
				{
					IsActive = TRUE;

					D2D1_ELLIPSE handmark;
					D2D1::Matrix3x2F translation;
					ISolidColorBrush *border_brush = BorderBrush.Get( );

					if ( ri->Nodes[ 0 ][ i ].opennessState ==
						PXCGesture::GeoNode::LABEL_CLOSE )
						border_brush = CloseBorderBrush.Get( ),
						IsDragging = TRUE;
					else IsDragging = FALSE;

					::Nena::Vector2 offset =
					{
						ScreenSize->Width * (320.0f - ri->Nodes[ 0 ][ i ].positionImage.x) / 320.0f,
						ScreenSize->Height * ri->Nodes[ 0 ][ i ].positionImage.y / 240.0f
					};

					offset.x -= Radius * 0.5f;
					offset.y -= Radius * 0.5f;
					offset.x -= ScreenSize->Width * 0.3f;
					offset.y -= ScreenSize->Height * 0.3f;
					offset.x *= 3.0f;
					offset.y *= 3.0f;

					/*	offset.x -= offset.x * 0.5f;
						offset.y -= offset.y * 0.5f;
						offset.x *= 2.0f;
						offset.y *= 2.0f;*/

					translation = D2D1::Matrix3x2F::Translation( offset.x, offset.y );

					if ( IsMovable )
					{
						LastPosition.x = translation._31;
						LastPosition.y = translation._32;
					}

					if ( IsVisible )
					{
						handmark = D2D1::Ellipse(
							D2D1::Point2F( Radius * 0.5f, Radius * 0.5f ),
							Radius * 0.5f, Radius * 0.5f
							);

						Overlay->Context->SetTransform( translation );
						Overlay->Context->DrawEllipse( handmark, border_brush, Radius * 0.03f );
						Overlay->Context->FillEllipse( handmark, FillBrush.Get( ) );
						Overlay->Context->SetTransform( D2D1::Matrix3x2F::Identity( ) );
					}

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
	CloseBorderBrush->SetOpacity( 0.5f );
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::WhiteSmoke ),
		BorderBrush.ReleaseAndGetAddressOf( )
		);
	BorderBrush->SetOpacity( 0.5f );
	Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF( 0.15f, 0.15f, 0.15f ),
		FillBrush.ReleaseAndGetAddressOf( )
		);
	FillBrush->SetOpacity( 0.2f );
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
