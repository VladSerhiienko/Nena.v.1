#include "app.precompiled.h"
#include "itv.oasis.home.h"
#include "itv.oasis.ui.pointers.h"
#include "itv.oasis.ui.round.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_HOME_STARTSCREEN_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_HOME_STARTSCREEN_INCLUDED__

struct InteractiveTV::Oasis::Home::StartScreen
	: public InteractiveTV::Oasis::Home::State
{
	typedef ::tm TimeGeneric;
	typedef ::time_t TimeUnix;
	typedef ::std::string String8;
	typedef ::std::wstring String, String16;
	typedef ::Nena::Animation::EasingStage Stage;
	typedef ::Nena::Animation::EasingFunctionPair<
		::Nena::Animation::EasingFunctionType::kExpOut,  
		::Nena::Animation::EasingFunctionType::kExpOut>
		SinusoidalInInEasingFunctionPair;

	typedef struct TextLabel
	{
		::FLOAT FontSizeOrigin;
		StartScreen::String TimeText;
		StartScreen::String DateText;
		StartScreen::TimeUnix Timeux;
		StartScreen::TimeGeneric Time;
		String::value_type TimeTextBuffer[ 128 ];
		::InteractiveTV::Ui::Base::Text Txt;

		void Init( );
		void Quit( );
		void CreateDeviceResources( );
		void CreateWindowSizeDependentResources( );
		void OnFrameMove( );

	} TextLabel;


	StartScreen::TextLabel TimeDateLabel;
	::InteractiveTV::Ui::Round::Dialog Navdialog;
	::InteractiveTV::Ui::Pointers *Pointers = nullptr;
	::InteractiveTV::Oasis::Home::State *Hub = nullptr;
	StartScreen::SinusoidalInInEasingFunctionPair Easing;

	StartScreen( Oasis::Home * );
	~StartScreen( );

	StartScreen::Stage GetEasingStage( 
		);
	void OnDialogButtonSelected(
		::InteractiveTV::Ui::Round::Dialog *,
		::InteractiveTV::Ui::Round::Dialog::Button *
		);

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

	virtual void OnGestureReceived(
		Remote::Input::GestureAppMessageArg
		) override;

};

#endif // !__NENA_INTERACTIVE_TV_OASIS_HOME_STARTSCREEN_INCLUDED__
