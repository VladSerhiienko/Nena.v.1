#include "app.precompiled.h"
#include "itv.oasis.web.h"
#include "itv.oasis.air.h"

#define _Oasis_web "web/"

InteractiveTV::Project::Oasis::Web::AppBase::List InteractiveTV::Project::Oasis::Web::s_kids;

InteractiveTV::Project::Oasis::Web::AppBase::AppBase(
	Oasis::Web::AppBase *master
	)
	: Master( master )
	, View( nullptr )
	, Session( nullptr )
{
	s_kids.push_back( this );
	SessionPreferencies.enable_plugins = true;
	SessionPreferencies.enable_app_cache = true;
	SessionPreferencies.enable_smooth_scrolling = true;
	SessionPreferencies.enable_gpu_acceleration = true;

	SessionPreferencies.user_stylesheet = Awesomium::WSLit( 
		"::-webkit-scrollbar { visibility: hidden; }"
		);
}

InteractiveTV::Project::Oasis::Web::AppBase::~AppBase( )
{
	DestroyCoreObjects( );
}

void InteractiveTV::Project::Oasis::Web::AppBase::SetName( Oasis::String name )
{
}

void InteractiveTV::Project::Oasis::Web::AppBase::CreateCoreObjects( )
{
	auto xsc = &Oasis::GetForCurrentThread( )->Context;

	if ( !View && !Session )
	{
		Session = xsc->Web->m_core->CreateWebSession(
			Awesomium::ToWebString( xsc->Path ),
			SessionPreferencies
			);
	}

	if ( !View )
	{
		if ( !Width ) Width = (INT32) xsc->Engine->Graphics.d3d.ActualRenderTargetSize.Width;
		if ( !Height ) Height = (INT32) xsc->Engine->Graphics.d3d.ActualRenderTargetSize.Height;

		View = xsc->Web->m_core->CreateWebView(
			Width, Height, Session,
			Awesomium::kWebViewType_Offscreen
			);
	}
}

void InteractiveTV::Project::Oasis::Web::AppBase::DestroyCoreObjects( )
{
	if ( View ) View->Destroy( ), View = nullptr;
	if ( Session ) Session->Release( ), Session = nullptr;
}

InteractiveTV::Project::Oasis::Web *
InteractiveTV::Project::Oasis::Web::GetForCurrentThread( )
{
	static InteractiveTV::Project::Oasis::Web s_web; return &s_web;
}

InteractiveTV::Project::Oasis::Web::Web( )
{
	typedef BOOL( WINAPI * IsWow64ProcessFunctionPointer ) (HANDLE, PBOOL);

	Name = _Oasis_origin _Oasis_web;

	auto xsc = &Oasis::GetForCurrentThread( )->Context;
	xsc->Oasis->AssignId( this );

	_Oasis_air_grabot( this, OasisAirMsg::kInfo, "ctor" );

	Oasis::String log_path = xsc->Path + ".osmlog.txt";
	m_core_configuration.log_level = Awesomium::kLogLevel_Verbose;
	m_core_configuration.log_path = Awesomium::WSLit( log_path.c_str( ) );
	m_core_configuration.plugin_path = Awesomium::WSLit(
		"C:/Windows/System32/Macromed/Flash/"
		);

	BOOL is_wow64_result = FALSE;
	auto is_wow64_function = (IsWow64ProcessFunctionPointer) GetProcAddress(
		GetModuleHandleA( "kernel32" ), "IsWow64Process"
		);
	if ( NULL != is_wow64_function )
	if ( is_wow64_function( GetCurrentProcess( ), &is_wow64_result ) )
	if ( is_wow64_result ) m_core_configuration.plugin_path = Awesomium::WSLit(
		"C:/Windows/SysWOW64/Macromed/Flash/"
		);
}

InteractiveTV::Project::Oasis::Web::~Web( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"dtor"
		);
}

void InteractiveTV::Project::Oasis::Web::Init( )
{
	m_core = Awesomium::WebCore::Initialize(
		m_core_configuration
		);

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"init"
		);
}

void InteractiveTV::Project::Oasis::Web::Quit( )
{
	//for (auto &kid : s_kids) delete kid, kid = nullptr; s_kids.clear();
	if ( m_core ) Awesomium::WebCore::Shutdown( ), m_core = nullptr;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"quit"
		);
}

void InteractiveTV::Project::Oasis::Web::OnFrameMove( )
{
	if ( m_core ) m_core->Update( );
}



