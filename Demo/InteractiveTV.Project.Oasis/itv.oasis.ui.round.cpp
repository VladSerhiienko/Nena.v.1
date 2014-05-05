#include "app.precompiled.h"
#include "itv.oasis.ui.round.h"

#define _Oasis_round_button "ui/roundbutton/"

InteractiveTV::Ui::Round::ImageButton::ImageButton( )
: State( nullptr )
{
	Name = _Oasis_origin _Oasis_round_button;

	button_icon.AcquireGlobals( );
	button_form.AcquireGlobals( );
	button_stroke.AcquireGlobals( );
	button_icon.img_file = "./Assets/Media-Player.png";
}

InteractiveTV::Ui::Round::ImageButton::~ImageButton( )
{
}

void InteractiveTV::Ui::Round::ImageButton::Init(
	)
{
	button_icon.AcquireGlobals( );
	button_form.AcquireGlobals( );
	button_ring.AcquireGlobals( );
	button_stroke.AcquireGlobals( );

	button_ring.element_ancestor = &button_form;
	button_icon.element_ancestor = &button_form;
	button_stroke.element_ancestor = &button_form;

	button_icon.element_dimensions.x = 0.17f;
	button_icon.element_dimensions.y = 0.17f;
	button_form.element_dimensions.x = 0.12f;
	button_form.element_dimensions.y = 0.12f;
	button_stroke.circle_stroke_width = 0.01f;
	button_stroke.element_dimensions.x = 0.11f;
	button_stroke.element_dimensions.y = 0.11f;
	button_ring.element_dimensions.x = 0.11f;
	button_ring.element_dimensions.y = 0.11f;
	button_ring.progress_arc_stroke_width = 0.01f;
	button_ring.progress_arc_start_angle = 90.0f;
	button_ring.progress_arc_end_angle = 450.0f;
	button_ring.progress_value = 0.0f;

	button_form.circle_fill_color = ::D2D1::ColorF( 0.11f, 0.11f, 0.11f );
	button_stroke.circle_fill_color = ::D2D1::ColorF( 0.12f, 0.12f, 0.12f );
	button_form.circle_border_color = ::D2D1::ColorF( 0.15f, 0.15f, 0.15f );
	button_stroke.circle_border_color = ::D2D1::ColorF( 0.15f, 0.15f, 0.15f );
	button_ring.progress_arc_color = ::D2D1::ColorF( 0.35f, 0.35f, 0.35f );
}

void InteractiveTV::Ui::Round::ImageButton::Quit(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::Resume(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::Suspend(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::OnResized(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::OnFrameMove(
	)
{
	button_form.FillCircle( );
	button_stroke.DrawFillCircle( );
	button_icon.DrawIcon( );
	button_ring.Draw( );
}

void InteractiveTV::Ui::Round::ImageButton::CreateDeviceResources(
	)
{
	button_form.CreateDeviceResources( );
	button_icon.CreateDeviceResources( );
	button_stroke.CreateDeviceResources( );
	button_ring.CreateDeviceResources( );
}

void InteractiveTV::Ui::Round::ImageButton::CreateDeviceIndependentResources(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::CreateWindowSizeDependentResources(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::DiscardDeviceResources(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::DiscardWindowSizeDependentResources(
	)
{
}

void InteractiveTV::Ui::Round::ImageButton::OnGestureReceived(
	Remote::Input::GestureAppMessageArg
	)
{
}

InteractiveTV::Ui::Round::Dialog::Button::Button( Dialog *dlg ) : host_dialog( dlg )
{
}

void InteractiveTV::Ui::Round::Dialog::Button::Init(
	UINT32 i,
	UINT32 c
	)
{
	button_control.button_form.AcquireGlobals( );
	button_control.button_icon.AcquireGlobals( );
	button_control.button_ring.AcquireGlobals( );
	button_control.button_stroke.AcquireGlobals( );
	button_control.Init( );

	if ( i )
	{
		auto angular_offset = -360.0f
			* (::FLOAT) (i - 1)
			/ (::FLOAT) (c - 1)
			+ (c != 3
			? -90.0f
			: 0.0f);

		button_control.button_form.element_position_ts.x = 0.3f;
		button_control.button_form.element_angular_offset_ts = angular_offset;
		button_control.button_form.element_ancestor = &host_dialog->dialog_form;
	}
	else
	{
		button_control.button_form.element_dimensions.x *= 0.72f;
		button_control.button_form.element_dimensions.y *= 0.72f;
		button_control.button_icon.element_dimensions.x *= 0.7f;
		button_control.button_icon.element_dimensions.y *= 0.7f;
		button_control.button_ring.element_dimensions.x *= 0.7f;
		button_control.button_ring.element_dimensions.y *= 0.7f;
		button_control.button_stroke.element_dimensions.x *= 0.7f;
		button_control.button_stroke.element_dimensions.y *= 0.7f;
	}

	auto total = 1.0f;
	auto factor = 0.1f;
	auto factor_reversed = 1.0f - factor;

	button_easing_angular_offset.SetTotal( total );
	button_easing_centre_offset.SetTotal( total );
	button_easing_form_dimension.SetTotal( total );
	button_easing_icon_dimension.SetTotal( total );
	button_easing_ring_dimension.SetTotal( total );
	button_easing_stroke_dimension.SetTotal( total );

	button_easing_angular_offset.SuspendingValue.Params.Offset = button_control.button_form.element_angular_offset_ts;
	button_easing_angular_offset.SuspendingValue.Params.Distance = -90.0f;
	button_easing_angular_offset.ResumingValue.Params.Offset = button_control.button_form.element_angular_offset_ts + 90.0f;
	button_easing_angular_offset.ResumingValue.Params.Distance = -90.0f;

	button_easing_centre_offset.SuspendingValue.Params.Offset = button_control.button_form.element_position_ts.x;
	button_easing_centre_offset.SuspendingValue.Params.Distance = button_control.button_form.element_position_ts.x * -factor_reversed;
	button_easing_centre_offset.ResumingValue.Params.Offset = button_control.button_form.element_position_ts.x * factor;
	button_easing_centre_offset.ResumingValue.Params.Distance = button_control.button_form.element_position_ts.x * factor_reversed;

	button_easing_form_dimension.SuspendingValue.Params.Offset = button_control.button_form.element_dimensions.x;
	button_easing_form_dimension.SuspendingValue.Params.Distance = button_control.button_form.element_dimensions.x * -factor_reversed;
	button_easing_form_dimension.ResumingValue.Params.Offset = button_control.button_form.element_dimensions.x * factor;
	button_easing_form_dimension.ResumingValue.Params.Distance = button_control.button_form.element_dimensions.x * factor_reversed;

	button_easing_icon_dimension.SuspendingValue.Params.Offset = button_control.button_icon.element_dimensions.x;
	button_easing_icon_dimension.SuspendingValue.Params.Distance = button_control.button_icon.element_dimensions.x * -factor_reversed;
	button_easing_icon_dimension.ResumingValue.Params.Offset = button_control.button_icon.element_dimensions.x * factor;
	button_easing_icon_dimension.ResumingValue.Params.Distance = button_control.button_icon.element_dimensions.x * factor_reversed;

	button_easing_ring_dimension.SuspendingValue.Params.Offset = button_control.button_ring.element_dimensions.x;
	button_easing_ring_dimension.SuspendingValue.Params.Distance = button_control.button_ring.element_dimensions.x * -factor_reversed;
	button_easing_ring_dimension.ResumingValue.Params.Offset = button_control.button_ring.element_dimensions.x * factor;
	button_easing_ring_dimension.ResumingValue.Params.Distance = button_control.button_ring.element_dimensions.x * factor_reversed;

	button_easing_stroke_dimension.SuspendingValue.Params.Offset = button_control.button_stroke.element_dimensions.x;
	button_easing_stroke_dimension.SuspendingValue.Params.Distance = button_control.button_stroke.element_dimensions.x * -factor_reversed;
	button_easing_stroke_dimension.ResumingValue.Params.Offset = button_control.button_stroke.element_dimensions.x * factor;
	button_easing_stroke_dimension.ResumingValue.Params.Distance = button_control.button_stroke.element_dimensions.x * factor_reversed;

}

InteractiveTV::Ui::Round::Dialog::Stage
InteractiveTV::Ui::Round::Dialog::Button::GetEasingStage( )
{
	return button_easing_form_dimension.CurrentStage;
}


void InteractiveTV::Ui::Round::Dialog::Button::Quit( )
{
	button_control.Quit( );
}

void InteractiveTV::Ui::Round::Dialog::Button::Resume( )
{
	button_easing_angular_offset.Resume( );
	button_easing_centre_offset.Resume( );
	button_easing_form_dimension.Resume( );
	button_easing_icon_dimension.Resume( );
	button_easing_ring_dimension.Resume( );
	button_easing_stroke_dimension.Resume( );
}

void InteractiveTV::Ui::Round::Dialog::Button::Suspend( )
{
	button_easing_angular_offset.Suspend( );
	button_easing_centre_offset.Suspend( );
	button_easing_form_dimension.Suspend( );
	button_easing_icon_dimension.Suspend( );
	button_easing_ring_dimension.Suspend( );
	button_easing_stroke_dimension.Suspend( );
}

void InteractiveTV::Ui::Round::Dialog::Button::OnFrame(
	::Nena::Simulation::BasicTimer *timer
	)
{
	if ( GetEasingStage( ) == Stage::kSuspended ) return;

	button_easing_centre_offset += timer->Elapsed;
	button_easing_angular_offset += timer->Elapsed;
	button_easing_form_dimension += timer->Elapsed;
	button_easing_icon_dimension += timer->Elapsed;
	button_easing_ring_dimension += timer->Elapsed;
	button_easing_stroke_dimension += timer->Elapsed;

	button_control.button_form.element_position_ts.x = button_easing_centre_offset.OnFrame( );
	button_control.button_form.element_angular_offset_ts = button_easing_angular_offset.OnFrame( );

	button_control.button_form.element_dimensions.x = button_easing_form_dimension.OnFrame( );
	button_control.button_icon.element_dimensions.x = button_easing_icon_dimension.OnFrame( );
	button_control.button_ring.element_dimensions.x = button_easing_ring_dimension.OnFrame( );
	button_control.button_stroke.element_dimensions.x = button_easing_stroke_dimension.OnFrame( );

	button_control.button_form.element_dimensions.y = button_control.button_form.element_dimensions.x;
	button_control.button_icon.element_dimensions.y = button_control.button_icon.element_dimensions.x;
	button_control.button_ring.element_dimensions.y = button_control.button_ring.element_dimensions.x;
	button_control.button_stroke.element_dimensions.y = button_control.button_stroke.element_dimensions.x;

	if ( button_pointers->IsActive[ 0 ] &&
		GetEasingStage( ) == Stage::kResumed )
	{
		if ( button_pointers->IsDragging[ 0 ] )
		{
			auto distance = button_control.button_form.CalculateActualDistanceTo( button_pointers->Visuals->Uis[ 0 ] );
			if ( distance < 50.0f ) button_control.button_ring.progress_value += 0.015f;
			else button_control.button_ring.progress_value -= 0.025f;
		}
		else button_control.button_ring.progress_value -= 0.025f;
	}
	else button_control.button_ring.progress_value -= 0.025f;

	button_control.OnFrameMove( );

	if ( button_control.button_ring.progress_value >= 0.99f )
		button_control.button_ring.progress_value = 0.00f,
		host_dialog->OnButtonSelected( this );
}

InteractiveTV::Ui::Round::Dialog::Dialog( )
: Oasis::Home::State( nullptr )
{
}

InteractiveTV::Ui::Round::Dialog::~Dialog( )
{
}

void InteractiveTV::Ui::Round::Dialog::Init( )
{
	UINT32 button_index( 0 );
	UINT32 button_total( dialog_buttons.size( ) );

	dialog_form.AcquireGlobals( );
	dialog_stroke.AcquireGlobals( );
	dialog_ring.AcquireGlobals( );
	dialog_stroke.element_ancestor = &dialog_form;
	dialog_ring.element_ancestor = &dialog_form;
	dialog_timer = &dialog_form.cross_context->Timer;

	dialog_form.element_dimensions.x = 0.37f;
	dialog_form.element_dimensions.y = 0.37f;
	dialog_stroke.circle_stroke_width = 0.01f;
	dialog_stroke.element_dimensions.x = 0.36f;
	dialog_stroke.element_dimensions.y = 0.36f;
	dialog_ring.element_dimensions.x = 0.38f;
	dialog_ring.element_dimensions.y = 0.38f;
	dialog_ring.progress_arc_stroke_width = 0.01f;
	dialog_ring.progress_arc_start_angle = 90.0f;
	dialog_ring.progress_arc_end_angle = 450.0f;
	dialog_ring.progress_value = 0.0f;

	dialog_form.circle_fill_color = ::D2D1::ColorF( 0.125f, 0.125f, 0.125f );
	dialog_stroke.circle_fill_color = ::D2D1::ColorF( 0.12f, 0.12f, 0.12f );
	dialog_form.circle_border_color = ::D2D1::ColorF( 0.15f, 0.15f, 0.15f );
	dialog_stroke.circle_border_color = ::D2D1::ColorF( 0.15f, 0.15f, 0.15f );
	dialog_ring.progress_arc_color = ::D2D1::ColorF( 0.17f, 0.17f, 0.17f );

	auto total = 1.0f;
	auto factor = 0.1f;
	auto factor_reversed = 1.0f - factor;

	dialog_easing_form_dimension.SetTotal( total );
	dialog_easing_ring_dimension.SetTotal( total );
	dialog_easing_stroke_dimension.SetTotal( total );

	dialog_easing_form_dimension.SuspendingValue.Params.Offset = dialog_form.element_dimensions.x;
	dialog_easing_form_dimension.SuspendingValue.Params.Distance = dialog_form.element_dimensions.x * -factor_reversed;
	dialog_easing_form_dimension.ResumingValue.Params.Offset = dialog_form.element_dimensions.x * factor;
	dialog_easing_form_dimension.ResumingValue.Params.Distance = dialog_form.element_dimensions.x * factor_reversed;

	dialog_easing_ring_dimension.SuspendingValue.Params.Offset = dialog_ring.element_dimensions.x;
	dialog_easing_ring_dimension.SuspendingValue.Params.Distance = dialog_ring.element_dimensions.x * -factor_reversed;
	dialog_easing_ring_dimension.ResumingValue.Params.Offset = dialog_ring.element_dimensions.x * factor;
	dialog_easing_ring_dimension.ResumingValue.Params.Distance = dialog_ring.element_dimensions.x * factor_reversed;

	dialog_easing_stroke_dimension.SuspendingValue.Params.Offset = dialog_stroke.element_dimensions.x;
	dialog_easing_stroke_dimension.SuspendingValue.Params.Distance = dialog_stroke.element_dimensions.x * -factor_reversed;
	dialog_easing_stroke_dimension.ResumingValue.Params.Offset = dialog_stroke.element_dimensions.x * factor;
	dialog_easing_stroke_dimension.ResumingValue.Params.Distance = dialog_stroke.element_dimensions.x * factor_reversed;

	for ( auto &button : dialog_buttons )
		button.Init(
		button_index++,
		button_total
		);

	Initted( this );
}

InteractiveTV::Ui::Round::Dialog::Stage
InteractiveTV::Ui::Round::Dialog::GetEasingStage( )
{
	return dialog_easing_form_dimension.CurrentStage;
}

void InteractiveTV::Ui::Round::Dialog::Quit( )
{
	for ( auto &button : dialog_buttons )
		button.Quit( );
	Quitted( this );
}

void InteractiveTV::Ui::Round::Dialog::Resume( )
{
	dialog_easing_form_dimension.Resume( );
	dialog_easing_ring_dimension.Resume( );
	dialog_easing_stroke_dimension.Resume( );
	for ( auto &button : dialog_buttons )
		button.Resume( );
}

void InteractiveTV::Ui::Round::Dialog::Suspend( )
{
	dialog_easing_form_dimension.Suspend( );
	dialog_easing_ring_dimension.Suspend( );
	dialog_easing_stroke_dimension.Suspend( );
	for ( auto &button : dialog_buttons )
		button.Suspend( );
}

void InteractiveTV::Ui::Round::Dialog::OnResized( )
{
}

void InteractiveTV::Ui::Round::Dialog::OnFrameMove( )
{
	if ( GetEasingStage( ) == Stage::kSuspended ) return;

	dialog_easing_form_dimension += dialog_timer->Elapsed;
	dialog_easing_ring_dimension += dialog_timer->Elapsed;
	dialog_easing_stroke_dimension += dialog_timer->Elapsed;

	dialog_form.element_dimensions.x = dialog_easing_form_dimension.OnFrame( );
	dialog_ring.element_dimensions.x = dialog_easing_ring_dimension.OnFrame( );
	dialog_stroke.element_dimensions.x = dialog_easing_stroke_dimension.OnFrame( );

	dialog_form.element_dimensions.y = dialog_form.element_dimensions.x;
	dialog_ring.element_dimensions.y = dialog_ring.element_dimensions.x;
	dialog_stroke.element_dimensions.y = dialog_stroke.element_dimensions.x;

	dialog_form.DrawFillCircle( );
	dialog_stroke.DrawFillCircle( );
	dialog_ring.Draw( );

	for ( auto &button : dialog_buttons )
		button.OnFrame( dialog_timer );
}

void InteractiveTV::Ui::Round::Dialog::CreateDeviceResources( )
{
	dialog_form.CreateDeviceResources( );
	dialog_stroke.CreateDeviceResources( );
	dialog_ring.CreateDeviceResources( );
	for ( auto &button : dialog_buttons )
		button.button_control.CreateDeviceResources( );
}

void InteractiveTV::Ui::Round::Dialog::CreateDeviceIndependentResources( )
{
	for ( auto &button : dialog_buttons )
		button.button_control.CreateDeviceIndependentResources( );
}

void InteractiveTV::Ui::Round::Dialog::CreateWindowSizeDependentResources( )
{
	for ( auto &button : dialog_buttons )
		button.button_control.CreateWindowSizeDependentResources( );
}

void InteractiveTV::Ui::Round::Dialog::DiscardDeviceResources( )
{
	for ( auto &button : dialog_buttons )
		button.button_control.DiscardDeviceResources( );
}

void InteractiveTV::Ui::Round::Dialog::DiscardWindowSizeDependentResources( )
{
	for ( auto &button : dialog_buttons )
		button.button_control.DiscardWindowSizeDependentResources( );
}

void InteractiveTV::Ui::Round::Dialog::AddButton(
	::std::string img_file
	)
{
	Button new_button( this );
	new_button.button_control.button_icon.img_file = img_file;
	new_button.button_pointers = dialog_pointers;
	new_button.button_id = dialog_buttons.size( );
	dialog_buttons.push_back( new_button );
}

void InteractiveTV::Ui::Round::Dialog::OnButtonSelected(
	Button *sender
	)
{
	Suspend( );
	button_selected( this, sender );
}


void InteractiveTV::Ui::Round::Dialog::OnGestureReceived(
	Remote::Input::GestureAppMessageArg
	)
{

}

void InteractiveTV::Ui::Round::Dialog::BindTo( ::InteractiveTV::Ui::Base::Element *scene )
{
	dialog_form.element_ancestor = scene;
}