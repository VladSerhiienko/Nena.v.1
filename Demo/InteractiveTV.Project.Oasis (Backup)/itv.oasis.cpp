#include "app.precompiled.h"
#include "itv.oasis.home.h"
#include "itv.oasis.air.h"

struct InteractiveTV::Project::Oasis::Implementation
	: public InteractiveTV::Project::Oasis::Object
{
	static ::INT16 s_next_id;

	Oasis::Home *home;

	void AssignId( Oasis::Object *obj )
	{
		obj->Id = s_next_id++;
	}

	Implementation( )
		: home( new Oasis::Home )
	{
		AssignId( this );
		Name = _Oasis_origin;
		Name += "impl/";
		_Oasis_air_grabot( this, OasisAirMsg::kInfo, "impl ctor" );
	}

	~Implementation( )
	{
		delete home, home = NULL;
	}
};

::INT16 InteractiveTV::Project::Oasis::Implementation::s_next_id = 0;

void InteractiveTV::Project::Oasis::AssignId( Oasis::Object *object )
{
	impl->AssignId( object );
}

InteractiveTV::Project::Oasis::Shared::Shared( )
{
	using namespace Nena;
	using namespace Nena::Application;

	Oasis = Oasis::GetForCurrentThread( );
	Air = Oasis::Air::GetForCurrentThread( );

	Name = _Oasis_origin;
	Name += "shared/";

	_Oasis_air_grabot( this, OasisAirMsg::kInfo, "ctor" );

	Oasis::GenerateUuid( Uuid );
	Path = "./.oasis/";
	UniquePath = "./.oasis/{" + Uuid + "}/";
	Oasis::CreateFolder( Path );

	App = App::GetForCurrentThread( );

	App->View.Fullscreen = FALSE;
	App->View.Width = 1200;
	App->View.Height = 800;
	App->View.Style = Window::GameWindowStyle;
	App->EnableCrtChecks( );
	App->MissingInit( App );
	App->View.UserData = this;
	App->UserData = this;
	App->OnInit = NULL;

	Engine = Engine::GetForCurrentThread( );
	Engine->Graphics.d3d.Windowed = !App->View.Fullscreen;

	Web = Web::GetForCurrentThread( );

	Engine->Graphics.d3d.DeviceLost += Nena_Delegate_MakeBind( Oasis, &Oasis::OnDeviceLost );
	Engine->Graphics.d3d.DeviceRestored += Nena_Delegate_MakeBind( Oasis, &Oasis::OnDeviceRestored );
	Engine->Graphics.d3d.SwapchainResizing += Nena_Delegate_MakeBind( Oasis, &Oasis::OnSwapchainResizing );
	Engine->Graphics.d3d.SwapchainResized += Nena_Delegate_MakeBind( Oasis, &Oasis::OnSwapchainResized );

	Web->Init( );
	Engine->Init( );
}

InteractiveTV::Project::Oasis::Oasis( )
: impl( !impl ? new Implementation : impl )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "ctor" );

	Context.App->QuitRequested += Nena_Delegate_MakeBind(
		this, &Oasis::OnAppQuitRequested
		);
}

InteractiveTV::Project::Oasis::~Oasis( )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "dtor" );
	if ( impl ) delete impl, impl = nullptr;
}

void InteractiveTV::Project::Oasis::Run( )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "run" );
	impl->home->Init( );

	Context.App->OnUpdate = NULL;

	//Context.App->View.SizeChanged += Nena_Delegate_MakeBind(this, &Oasis::OnSizeChanged);
	//Context.App->View.ResizedMoved += Nena_Delegate_MakeBind(this, &Oasis::OnSizeChanged);
	//Context.App->View.Toggled += Nena_Delegate_MakeBind(this, &Oasis::OnSizeChanged);

	Context.App->MessageLoop( );
}

InteractiveTV::Project::Oasis::State::State( Oasis::State *master )
: Context( &Oasis::GetForCurrentThread( )->Context )
, Master( master )
{
	Context->Oasis->AssignId( this );
	if ( master ) Name = master->Name;

	Resumed += Nena_Delegate_MakeBind( Context->Oasis, &Oasis::OnStateResumed );
	Quitted += Nena_Delegate_MakeBind( Context->Oasis, &Oasis::OnStateQuitted );
	Suspended += Nena_Delegate_MakeBind( Context->Oasis, &Oasis::OnStateSuspended );
	Suspended += Nena_Delegate_MakeBind( Context->Oasis, &Oasis::OnStateDestroyed );

	if ( master )
	{
		Quitted += Nena_Delegate_MakeBind( master, &Oasis::State::OnStateQuitted );
		Resumed += Nena_Delegate_MakeBind( master, &Oasis::State::OnStateResumed );
		Suspended += Nena_Delegate_MakeBind( master, &Oasis::State::OnStateSuspended );
	}
}

InteractiveTV::Project::Oasis::State::~State( )
{
	Destoyed( this );
}

void InteractiveTV::Project::Oasis::OnFrameMove( )
{
	Context.Web->OnFrameMove( );

	Context.Engine->BeginScene( );
	impl->home->OnFrameMove( );
	Context.Engine->EndScene( );
}

void InteractiveTV::Project::Oasis::OnToggleFullscreen( )
{

	if ( Context.Engine->Graphics.d3d.Windowed )
	{
		_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "toggle fullscreen mode" );

		Context.Engine->Graphics.d3d.Windowed = FALSE;
		Context.Engine->Graphics.d3d.SetLogicalSize( 0 );
	}
	else
	{
		_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "toggle windowed mode" );

		Context.Engine->Graphics.d3d.Windowed = TRUE;
		Context.Engine->Graphics.d3d.SetLogicalSize(
		{
			(::FLOAT) Context.App->View.Width,
			(::FLOAT) Context.App->View.Height
		} );
	}
}

void InteractiveTV::Project::Oasis::OnDeviceLost( ::Nena::Graphics::DeviceResources * )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "device lost" );

	if ( impl ) impl->home->DiscardDeviceResources( );

	// release device dependent resources
	Context.Engine->Graphics.d2d.TargetBitmap = nullptr;
	Context.Engine->Graphics.d2d.Context = nullptr;
	Context.Engine->Graphics.d2d.Device = nullptr;
}

void InteractiveTV::Project::Oasis::OnDeviceRestored( ::Nena::Graphics::DeviceResources * )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "device restored" );

	// create device dependent resources
	Context.Engine->Graphics.d2d.CreateDeviceResources(
		&Context.Engine->Graphics.d3d
		);
	impl->home->CreateDeviceResources(
		);
}

void InteractiveTV::Project::Oasis::OnSwapchainResizing( ::Nena::Graphics::DeviceResources * )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "swapchain resizing" );

	// release window size dependent resources
	if ( impl ) impl->home->DiscardWindowSizeDependentResources( );

	Context.Engine->Graphics.d2d.TargetBitmap = nullptr;
	if ( Context.Engine->Graphics.d2d.Context.Get( ) )
		Context.Engine->Graphics.d2d.Context->SetTarget( nullptr );
}

void InteractiveTV::Project::Oasis::OnSwapchainResized( ::Nena::Graphics::DeviceResources * )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "swapchain resized" );

	// create window size dependent resources

	if ( Context.Engine->Graphics.d2d.Device.Get( ) )
	{
		Context.Engine->Graphics.d2d.SetDpi(
			&Context.Engine->Graphics.d3d,
			Context.Engine->Graphics.d3d.Dpi.x,
			Context.Engine->Graphics.d3d.Dpi.y
			);

		if ( impl ) impl->home->OnResized( );
	}
}


void InteractiveTV::Project::Oasis::OnSizeChanged(
	::USHORT renderTargetWidth, ::USHORT renderTargetHeight
	)
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "swapchain adjusting" );

	if ( !Context.Engine->Graphics.d3d.Windowed )
	{
		if ( renderTargetWidth != 0 &&
			renderTargetHeight != 0 &&
			renderTargetWidth != Context.App->View.Width &&
			renderTargetHeight != Context.App->View.Height )
		{
			Context.Engine->Graphics.d3d.Swapchain->ResizeBuffers(
				Context.Engine->Graphics.d3d.SwapchainBufferCount,
				renderTargetWidth, renderTargetHeight,
				Context.Engine->Graphics.d3d.RenderTargetFormat,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				);
		}
	}
	else
	{
		if ( renderTargetWidth != 0 &&
			renderTargetHeight != 0 )
		{
			Context.Engine->Graphics.d3d.Swapchain->ResizeBuffers(
				Context.Engine->Graphics.d3d.SwapchainBufferCount,
				renderTargetWidth, renderTargetHeight,
				Context.Engine->Graphics.d3d.RenderTargetFormat,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				);
		}
	}
}

void InteractiveTV::Project::Oasis::OnGestureReceived( _In_::UINT32 key )
{
	impl->home->OnGestureReceived( key );
}

#pragma region State changes

void InteractiveTV::Project::Oasis::OnStateQuitted(
	Oasis::State *state
	)
{

}

void InteractiveTV::Project::Oasis::OnStateResumed(
	Oasis::State *state
	)
{

}

void InteractiveTV::Project::Oasis::OnStateSuspended(
	Oasis::State *state
	)
{

}

void InteractiveTV::Project::Oasis::OnStateDestroyed(
	Oasis::State *state
	)
{
	OutputDebugStringA( "InteractiveTV::Project::Oasis::OnStateDestroyed()\n" );
	OutputDebugStringA( "\t" ); OutputDebugStringA( state->GetNameCcstr( ) ); OutputDebugStringA( "\n" );
}

#pragma endregion

void InteractiveTV::Project::Oasis::Quit( )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "quit" );

	OnQuit( );
	if ( Context.App->View.Raw ) Context.App->View.Close( );
	else ::PostQuitMessage( EXIT_SUCCESS );
}
void InteractiveTV::Project::Oasis::OnQuit( )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "on quit" );

	impl->home->Quit( );
	Context.Web->Quit( );
	Context.Engine->Quit( );
}
void InteractiveTV::Project::Oasis::OnAppQuitRequested( ::Nena::App *app )
{
	_Oasis_air_grabot( impl, OasisAirMsg::kInfo, "on app quit requested" );

	OnQuit( );
}

void InteractiveTV::Project::Oasis::OnKeyPressed( ::UINT32 k )
{
	return impl->home->OnKeyPressed( k );
}

void InteractiveTV::Project::Oasis::OnKeyReleased( ::UINT32 k )
{
	return impl->home->OnKeyReleased( k );
}

void InteractiveTV::Project::Oasis::OnMouseMoved( ::FLOAT x, ::FLOAT y )
{
	return impl->home->OnMouseMoved( x, y );
}

void InteractiveTV::Project::Oasis::OnMouseLBPressed( ::FLOAT x, ::FLOAT y )
{
	return impl->home->OnMouseLBPressed( x, y );
}

void InteractiveTV::Project::Oasis::OnMouseRBPressed( ::FLOAT x, ::FLOAT y )
{
	return impl->home->OnMouseRBPressed( x, y );
}

void InteractiveTV::Project::Oasis::OnMouseLBReleased( ::FLOAT x, ::FLOAT y )
{
	return impl->home->OnMouseLBReleased( x, y );
}

void InteractiveTV::Project::Oasis::OnMouseRBReleased( ::FLOAT x, ::FLOAT y )
{
	return impl->home->OnMouseRBReleased( x, y );
}

#pragma region Utility

void InteractiveTV::Project::Oasis::GenerateUuid(
	Oasis::String &path
	)
{
	::UUID uuid;
	::RPC_CSTR rpc_cstring;
	::RPC_STATUS rpc_status;
	::CHAR uuid_buffer[ MAX_PATH ];

	rpc_status = ::UuidCreate( &uuid );
	rpc_status = ::UuidToStringA( &uuid, &rpc_cstring );

	sprintf_s( uuid_buffer, "%s", rpc_cstring );
	path = uuid_buffer;

	::RpcStringFreeA( &rpc_cstring );
}

::BOOL InteractiveTV::Project::Oasis::CreateFolder(
	Oasis::String directory
	)
{
	::DWORD fileAttributes = ::GetFileAttributesA( directory.c_str( ) );
	if ( fileAttributes == INVALID_FILE_ATTRIBUTES )
	{
		String::size_type slashIndex = directory.find_last_of( "\\/" );
		if ( slashIndex != String::npos ) CreateFolder( directory.substr( 0, slashIndex ) );
		return ::CreateDirectoryA( directory.c_str( ), nullptr );
	}
	else
	{
		bool isDirectoryOrJunction =
			((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
			((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
		return isDirectoryOrJunction;
	}
}

#pragma endregion
