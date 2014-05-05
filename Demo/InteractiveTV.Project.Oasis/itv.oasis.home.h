#include "app.precompiled.h"
#include "itv.remote.control.h"
#include "itv.oasis.ui.base.h"
#include "itv.oasis.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_HOME_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_HOME_INCLUDED__

struct InteractiveTV::Oasis::Home
	: public Oasis::State
	, public Remote::Input::Feed
{
	struct HubScreen;
	struct SignUpScreen;
	struct StartScreen;

	typedef struct State
		: public Oasis::State
	{
		typedef std::vector<Oasis::Home::State *> List;

		virtual void OnResized( ) = 0; 
		virtual void OnFrameMove( ) = 0;
		virtual void CreateDeviceResources( ) = 0;
		virtual void CreateDeviceIndependentResources( ) = 0;
		virtual void CreateWindowSizeDependentResources( ) = 0;
		virtual void DiscardDeviceResources( ) = 0;
		virtual void DiscardWindowSizeDependentResources( ) = 0;
		virtual void OnGestureReceived( Remote::Input::GestureAppMessageArg ) {}
		::InteractiveTV::Ui::Base::Element *GetScene( ) { return &Scene; }

		State( Oasis::Home *home ) : Oasis::State( home ), Host( home ) {}
		virtual ~State( ) {}

	protected:
		
		::InteractiveTV::Ui::Base::Element Scene;
		Oasis::Home *Host;

	} State;

	Home( );
	virtual ~Home( );

	void Init( );
	void Quit( );

	void OnFrameMove( );
	void OnBeginFrameMove( );
	void OnEndFrameMove( );

	void CreateDeviceResources( );
	void CreateDeviceIndependentResources( );
	void CreateWindowSizeDependentResources( );
	void DiscardDeviceResources( );
	void DiscardWindowSizeDependentResources( );

	Oasis::Shared *Context = nullptr;

	::BOOL IsResizing = FALSE;
	::Nena::Graphics::OverlayResources *Overlay = nullptr;
	::Nena::Graphics::Resources::Direct2DDrawingStateBlock BlockState = nullptr;

	virtual void Resume( ) override;
	virtual void Suspend( ) override;
	virtual void OnStateQuitted( _In_ Oasis::State * ) override;
	virtual void OnStateResumed( _In_ Oasis::State * ) override;
	virtual void OnStateSuspended( _In_ Oasis::State * ) override;

	void OnResized( );
	void OnKeyPressed( _In_::UINT32 );
	void OnKeyReleased( _In_::UINT32 );
	void OnGestureReceived( _In_::UINT32 );
	void OnMouseMoved( _In_::FLOAT, _In_::FLOAT );
	void OnMouseLBPressed( _In_::FLOAT, _In_::FLOAT );
	void OnMouseRBPressed( _In_::FLOAT, _In_::FLOAT );
	void OnMouseLBReleased( _In_::FLOAT, _In_::FLOAT );
	void OnMouseRBReleased( _In_::FLOAT, _In_::FLOAT );

private:

	::Nena::Animation::SinusoidalInEasingFunction scene_transitions;
	::InteractiveTV::Ui::Base::Element scene;

	::InteractiveTV::Remote::Input remote_input;
	State::List states;
};

#endif // !__NENA_INTERACTIVE_TV_OASIS_HOME_INCLUDED__



