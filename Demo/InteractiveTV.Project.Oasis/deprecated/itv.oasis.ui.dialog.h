#include "app.precompiled.h"
#include "itv.oasis.ui.button.h"

#include "nena.bitmap.helper.h"
#include "nena.animation.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_UI_DIALOG_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_UI_DIALOG_INCLUDED__

namespace InteractiveTV
{
	namespace Ui
	{
		struct Dialog;
		struct Pointers;
	}
}

struct InteractiveTV::Ui::Dialog
	: public InteractiveTV::Oasis::Home::State
{
	enum Stage
	{
		kSuspending,
		kSuspended,
		kResuming,
		kResumed,
	};

	struct AnimationControl
	{
		Nena::Animation::ExponencialInEasingFunction SuspendingRadius;
		Nena::Animation::ExponencialOutEasingFunction ResumingRadius;
	};

	typedef std::string String;
	typedef ::D2D1_ARC_SEGMENT ArcSegment;
	typedef ::D2D1_ELLIPSE Ellipse;
	typedef ::D2D1_COLOR_F Color;
	typedef ::D2D1_RECT_F Rect;
	typedef ::Nena::Graphics::Resources::Size Size;
	typedef ::Nena::Graphics::Resources::Direct2DBitmap Bitmap;
	typedef ::Nena::Graphics::OverlayResources OverlayResources;
	typedef ::Nena::Graphics::Resources::Direct2DSolidColorBrush SolidColorBrush;
	typedef ::Nena::Event<void, Ui::Dialog *, Ui::Button *> Event;

	Oasis::Shared *Context = nullptr;
	Dialog::Size *ScreenSize = nullptr;
	Dialog::OverlayResources *Overlay = nullptr;

	FLOAT Radius;
	FLOAT Progress;
	Button::List Buttons;
	Dialog::Ellipse Panel;
	Dialog::Color PanelColor;
	Dialog::Color ProgressArcColor;
	Dialog::ArcSegment ProgressArc;
	Dialog::SolidColorBrush BorderBrush;
	Dialog::SolidColorBrush FillBrush;
	Dialog::Event ButtonSelected;

	Dialog::Stage CurrentStage;
	Dialog::AnimationControl Easing;
	InteractiveTV::Oasis::Home *Host;

	Dialog( InteractiveTV::Oasis::Home *home );
	virtual ~Dialog( );

	void Add( String icon );
	void Set( Ui::Pointers *instance );

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

#endif // !__NENA_INTERACTIVE_TV_OASIS_UI_DIALOG_INCLUDED__
