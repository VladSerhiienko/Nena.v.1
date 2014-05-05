#include "app.precompiled.h"
#include "itv.oasis.ui.base.h"
#include "itv.oasis.ui.pointers.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_UI_ROUND_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_UI_ROUND_INCLUDED__

namespace InteractiveTV
{
	namespace Ui
	{
		namespace Round
		{
			struct ImageButton;
			struct Dialog;
		}
	}
}

struct InteractiveTV::Ui::Round::ImageButton : public InteractiveTV::Oasis::Home::State
{
	::InteractiveTV::Ui::Base::Image button_icon;
	::InteractiveTV::Ui::Base::Circle button_form;
	::InteractiveTV::Ui::Base::Circle button_stroke;
	::InteractiveTV::Ui::Base::ProgressRing button_ring;

	ImageButton( );
	~ImageButton( );

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

struct InteractiveTV::Ui::Round::Dialog : public ::InteractiveTV::Oasis::Home::State
{
	typedef ::Nena::Animation::EasingStage Stage;
	typedef ::Nena::Animation::LinearEasingFunctionPair EasingRingPair;
	typedef ::Nena::Animation::ExponencialEasingFunctionPair EasingPair;
	typedef struct Button
	{
		Dialog *host_dialog = nullptr;
		Dialog::EasingPair button_easing_opacity;
		Dialog::EasingPair button_easing_centre_offset;
		Dialog::EasingPair button_easing_angular_offset;
		Dialog::EasingPair button_easing_form_dimension;
		Dialog::EasingPair button_easing_icon_dimension;
		Dialog::EasingPair button_easing_ring_dimension;
		Dialog::EasingPair button_easing_stroke_dimension;

		::DWORD32 button_id = DWORD_ERROR;
		::InteractiveTV::Ui::Pointers *button_pointers;
		::InteractiveTV::Ui::Round::ImageButton button_control;

		Button( Dialog * );

		void Init( UINT32 button_index, UINT32 button_total );
		void Quit( );
		void Resume( );
		void Suspend( );
		void OnFrame( ::Nena::Simulation::BasicTimer * );
		Dialog::Stage GetEasingStage( );

	} Button;
	typedef ::Nena::Event<void, Dialog *, Button *> Event;
	typedef ::std::vector<Button> ButtonList;

	Dialog::Event button_selected;
	Dialog::ButtonList dialog_buttons;
	Dialog::EasingPair dialog_easing_opacity;
	Dialog::EasingPair dialog_easing_form_dimension;
	Dialog::EasingPair dialog_easing_stroke_dimension;
	Dialog::EasingPair dialog_easing_ring_dimension;

	::Nena::Simulation::BasicTimer *dialog_timer = nullptr;
	::InteractiveTV::Ui::Pointers *dialog_pointers = nullptr;
	::InteractiveTV::Ui::Base::Circle dialog_form;
	::InteractiveTV::Ui::Base::Circle dialog_stroke;
	::InteractiveTV::Ui::Base::ProgressRing dialog_ring;

	Dialog( );
	~Dialog( );

	Dialog::Stage GetEasingStage( );
	void OnButtonSelected( Button *sender );
	void AddButton( ::std::string img_file );
	void BindTo( ::InteractiveTV::Ui::Base::Element *scene );

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

#endif //!__NENA_INTERACTIVE_TV_OASIS_UI_ROUND_INCLUDED__