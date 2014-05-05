#include "app.precompiled.h"
#include "itv.oasis.home.h"
#include "itv.oasis.ui.base.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_UI_POINTER_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_UI_POINTER_INCLUDED__

namespace InteractiveTV
{
	namespace Ui
	{
		struct Pointers;
	}
}

struct InteractiveTV::Ui::Pointers
	: public InteractiveTV::Oasis::Home::State
{
	typedef struct IVisuals : Oasis::Home::State
	{
		::BOOL IsDragTried[ 2 ];
		Ui::Base::Element *Uis[ 2 ];
		InteractiveTV::Ui::Pointers *Host = nullptr;
		IVisuals( InteractiveTV::Ui::Pointers *host )
			: State( NULL ) , Host( host )
		{
			IsDragTried[ 0 ] = IsDragTried[ 1 ] = FALSE;
			Uis[ 0 ] = Uis[ 1 ] = nullptr;
		}

	} IVisuals;
	typedef struct DefaultVisuals : public IVisuals
	{
		Ui::Base::Circle Landmark[ 2 ];
		DefaultVisuals( InteractiveTV::Ui::Pointers *host );

		virtual void Init( ) override;
		virtual void Quit( ) override;
		virtual void Resume( ) override;
		virtual void Suspend( ) override;
		virtual void OnResized( ) override;
		virtual void OnFrameMove( ) override;
		virtual void CreateDeviceResources( ) override;
		virtual void CreateDeviceIndependentResources( ) override;
		virtual void CreateWindowSizeDependentResources( ) override;
		virtual void DiscardDeviceResources( ) override;
		virtual void DiscardWindowSizeDependentResources( ) override;
		virtual void OnGestureReceived( Remote::Input::GestureAppMessageArg ) override;

	} DefaultVisuals;
	typedef enum Stage
	{
		//kSuspending,
		kSuspended,
		//kResuming,
		kResumed,
	} Stage;

	typedef std::string String;
	typedef ::D2D1_RECT_F Rect;
	typedef ::D2D1_COLOR_F Color;
	typedef ::D2D1_ELLIPSE Ellipse;
	typedef ::D2D1_ARC_SEGMENT ArcSegment;
	typedef ::Nena::Graphics::Resources::Size Size;
	typedef ::Nena::Graphics::Resources::Direct2DBitmap Bitmap;
	typedef ::Nena::Graphics::OverlayResources OverlayResources;
	typedef ::Nena::Graphics::Resources::Direct2DSolidColorBrush SolidColorBrush;
	typedef ::Nena::Graphics::Resources::Direct2DSolidColorBrush::InterfaceType ISolidColorBrush;

	Pointers::Stage CurrentStage;
	Pointers::Size *ScreenSize = nullptr;

	::FLOAT Radius;
	::BOOL IsActive[ 2 ]; //! per-frame updated
	::BOOL IsDragging[ 2 ]; //! per-frame updated
	::BOOL IsVisible[ 2 ]; //! user-defined
	::BOOL IsHoverable[ 2 ]; //! user-defined
	::UINT32 HandsToProcess = 1u; //! user-defined (has defaults)
	::UINT32 NodesToProcess = 10u; //! user-defined (has defaults)

	::FLOAT AdjustingFactor = 0.8f; //! user-defined (has defaults)
	::Nena::Vector2 DepthBufferDimensions; //! user-defined (has defaults)
	::Nena::Vector2 LastActualPosition[ 2 ]; //! per-frame updated
	::Nena::Vector2 LastRelativePosition[ 2 ]; //! per-frame updated
	::Nena::Vector2 LastAdjustedRelativePosition[ 2 ]; //! per-frame updated

	InteractiveTV::Oasis::Home *Host;
	Pointers::IVisuals *Visuals = nullptr;

	Pointers( InteractiveTV::Oasis::Home *home );
	virtual ~Pointers( );

	virtual void Init( ) override;
	virtual void Quit( ) override;
	virtual void Resume( ) override;
	virtual void Suspend( ) override;
	virtual void OnResized( ) override;
	virtual void OnFrameMove( ) override;
	virtual void CreateDeviceResources( ) override;
	virtual void CreateDeviceIndependentResources( ) override;
	virtual void CreateWindowSizeDependentResources( ) override;
	virtual void DiscardDeviceResources( ) override;
	virtual void DiscardWindowSizeDependentResources( ) override;

private:

	Pointers::DefaultVisuals DefaultVisuals;

};

#endif // !__NENA_INTERACTIVE_TV_OASIS_UI_POINTER_INCLUDED__
