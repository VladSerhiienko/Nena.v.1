#include "app.precompiled.h"
#include "itv.oasis.home.h"
#include "itv.oasis.home.hub.h"
#include "itv.oasis.ui.dialog.h"
#include "nena.animation.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_HOME_STARTSCREEN_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_HOME_STARTSCREEN_INCLUDED__

struct InteractiveTV::Project::Oasis::Home::StartScreen
	: public InteractiveTV::Project::Oasis::Home::State
{
	typedef enum Stage
	{
		kSuspending,
		kSuspended,
		kResuming,
		kResumed,

	} Stage;

	typedef struct AnimationControl
	{
		Nena::Animation::QuarticInEasingFunction SuspendingX;
		Nena::Animation::QuarticOutEasingFunction ResumingX;

	} TranslationControl;

	typedef ::tm TimeGeneric;
	typedef ::time_t TimeUnix;
	typedef ::std::string String8;
	typedef ::std::wstring String, String16;

	::FLOAT FontSizeOrigin;
	::BOOL IsResizing = FALSE;
	StartScreen::Stage CurrentStage;
	StartScreen::String TimeText;
	StartScreen::String DateText;
	StartScreen::TimeUnix Timeux;
	StartScreen::TimeGeneric Time;
	String::value_type TimeTextBuffer[ 128 ];
	Oasis::Shared *Context = nullptr;
	Oasis::Home::Hub *Hub = nullptr;

	Ui::Dialog Navigation;
	Ui::Pointers *Pointers;
	StartScreen::AnimationControl Easing;

	::Nena::Graphics::OverlayResources *Overlay = nullptr;
	::Nena::Graphics::Resources::Size *ScreenSize = nullptr;
	::Nena::Graphics::Resources::DirectWriteTextFormat TextFormat = nullptr;
	::Nena::Graphics::Resources::Direct2DSolidColorBrush TextBrush = nullptr;

	StartScreen( Oasis::Home * );
	~StartScreen( );

	void OnUpdateTimeText( );

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
	void OnButtonSelected( Ui::Dialog *, Ui::Button * );

	virtual void OnGestureReceived(
		Remote::Input::GestureAppMessageArg
		) override;

};

#endif // !__NENA_INTERACTIVE_TV_OASIS_HOME_STARTSCREEN_INCLUDED__
