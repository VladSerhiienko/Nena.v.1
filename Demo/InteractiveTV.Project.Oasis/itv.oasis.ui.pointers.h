#include "app.precompiled.h"
#include "itv.oasis.home.h"

#include "nena.bitmap.helper.h"
#include "nena.animation.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_UI_POINTER_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_UI_POINTER_INCLUDED__

namespace InteractiveTV
{
	namespace Ui
	{
		using namespace Project;
		struct Pointers;
	}
}

struct InteractiveTV::Ui::Pointers
	: public InteractiveTV::Project::Oasis::Home::State
{
	enum Stage
	{
		//kSuspending,
		kSuspended,
		//kResuming,
		kResumed,
	};

	struct AnimationControl
	{
		Nena::Animation::ExponencialInEasingFunction SuspendingX;
		Nena::Animation::ExponencialInEasingFunction SuspendingY;
		Nena::Animation::ExponencialOutEasingFunction ResumingX;
		Nena::Animation::ExponencialOutEasingFunction ResumingY;
	};

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

	Oasis::Shared *Context = nullptr;
	Pointers::Size *ScreenSize = nullptr;
	Pointers::OverlayResources *Overlay = nullptr;

	FLOAT Radius;
	Pointers::Stage CurrentStage;
	Pointers::SolidColorBrush FillBrush;
	Pointers::SolidColorBrush BorderBrush;
	Pointers::SolidColorBrush CloseBorderBrush;

	Pointers::AnimationControl Easing;
	InteractiveTV::Project::Oasis::Home *Host;

	Pointers( InteractiveTV::Project::Oasis::Home *home );
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
};

#endif // !__NENA_INTERACTIVE_TV_OASIS_UI_POINTER_INCLUDED__
