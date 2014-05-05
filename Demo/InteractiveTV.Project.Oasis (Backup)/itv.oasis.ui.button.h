#include "app.precompiled.h"
#include "itv.oasis.home.h"

#include "nena.bitmap.helper.h"
#include "nena.animation.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_UI_BUTTON_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_UI_BUTTON_INCLUDED__

namespace InteractiveTV
{
	namespace Ui
	{
		using namespace Project;
		struct Button;
		struct Pointers;
	}
}

struct InteractiveTV::Ui::Button
	: public InteractiveTV::Project::Oasis::Home::State
{
	typedef enum Stage
	{
		kSuspending,
		kSuspended,
		kResuming,
		kResumed,

	} Stage;

	typedef enum SelectionStage
	{
		kDeselecting,
		kDeselected,
		kSelecting,
		kSelected,
		kNone,

	} SelectionStage;

	typedef struct AnimationControl
	{
		::Nena::Animation::LinearEasingFunction ProgressArcSelecting;
		::Nena::Animation::LinearEasingFunction ProgressArcDeselecting;
		::Nena::Animation::ExponencialInEasingFunction SuspendingOffset;
		::Nena::Animation::ExponencialInEasingFunction SuspendingRadius;
		::Nena::Animation::ExponencialInEasingFunction SuspendingAngularOffset;
		::Nena::Animation::ExponencialOutEasingFunction ResumingOffset;
		::Nena::Animation::ExponencialOutEasingFunction ResumingRadius;
		::Nena::Animation::ExponencialOutEasingFunction ResumingAngularOffset;

	} AnimationControl;

	typedef ::D2D1_RECT_F Rect;
	typedef ::D2D1_COLOR_F Color;
	typedef ::D2D1_ELLIPSE Ellipse;
	typedef ::D2D1_ARC_SEGMENT ArcSegment;
	typedef ::std::string String;
	typedef ::std::vector<Button *> List;
	typedef ::Nena::Graphics::Resources::Size Size;
	typedef ::Nena::Graphics::OverlayResources OverlayResources;
	typedef ::Nena::Graphics::Resources::Direct2DBitmap Bitmap;
	typedef ::Nena::Graphics::Resources::Direct2DSolidColorBrush SolidColorBrush;
	typedef ::Nena::Graphics::Resources::Direct2DSolidColorBrush::InterfaceType ISolidColorBrush;

	Oasis::Shared *Context = nullptr;
	Button::Size *ScreenSize = nullptr;
	Button::OverlayResources *Overlay = nullptr;

	::UINT32 Index;
	::UINT32 PanelSize;
	::FLOAT Offset;
	::FLOAT Radius;
	::FLOAT Progress;
	::FLOAT AngularOffset;

	Button::Stage CurrentStage;
	Button::SelectionStage CurrentSelectionStage;

	Button::Bitmap Icon;
	Button::Bitmap LockTime;
	Button::Bitmap LockOpen;
	Button::String IconPath;

	Button::Ellipse Border;
	Button::ArcSegment ProgressArc;
	Button::Ellipse SelectedBorder;
	Button::SolidColorBrush BorderBrush;
	Button::SolidColorBrush FillBrush;
	Button::SolidColorBrush SelectedFillBrush;
	Button::SolidColorBrush SelectionProgressBrush;
	Button::SolidColorBrush DeselectionProgressBrush;

	::Nena::Vector2 CurrentPosition;
	Button::AnimationControl Easing;

	Ui::Pointers *Pointers;
	InteractiveTV::Project::Oasis::Home *Host;

	Button( InteractiveTV::Project::Oasis::Home *home );
	virtual ~Button( );

	void Select( );
	void Deselect( );

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

	void DrawProgressArc( );
};

#endif // !__NENA_INTERACTIVE_TV_OASIS_UI_BUTTON_INCLUDED__
