#include "app.precompiled.h"
#include "itv.oasis.web.h"

InteractiveTV::Project::Oasis::Web::AppBase::List InteractiveTV::Project::Oasis::Web::s_kids;

InteractiveTV::Project::Oasis::Web::AppBase::AppBase(Oasis::Web::AppBase *master)
	: Master(master)
	, View(nullptr)
	, Session(nullptr)
{
	s_kids.push_back(this);
	SessionPreferencies.enable_plugins = true;
	SessionPreferencies.enable_smooth_scrolling = true;
	SessionPreferencies.enable_gpu_acceleration = true;
	SessionPreferencies.user_stylesheet = Awesomium::WSLit("::-webkit-scrollbar { visibility: hidden; }");
}

InteractiveTV::Project::Oasis::Web::AppBase::~AppBase()
{
	DestroyCoreObjects();
}

void InteractiveTV::Project::Oasis::Web::AppBase::SetName(Oasis::String name)
{
}

void InteractiveTV::Project::Oasis::Web::AppBase::CreateCoreObjects()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Web::AppBase::CreateCoreObjects()\n");

	auto xsc = &Oasis::GetForCurrentThread()->Context;

	if (!View && !Session)
	{
		Session = xsc->Web->m_core->CreateWebSession(
		Awesomium::ToWebString(xsc->Path), 
		SessionPreferencies
		);
	}

	if (!View)
	{
		if (!Width) Width = xsc->Engine->Graphics.d3d.LogicalSize.Width;
		if (!Height) Height = xsc->Engine->Graphics.d3d.LogicalSize.Height;
		/*if (!Width) Width = xsc->App->View.Fullscreen
			? xsc->App->View.ScreenWidth
			: xsc->App->View.Width;
		if (!Height) Height = xsc->App->View.Fullscreen
			? xsc->App->View.ScreenHeight
			: xsc->App->View.Height;*/

		//Width *= 2;
		//Width /= 2;
		//Height *= 2;
		//Height /= 2;
		View = xsc->Web->m_core->CreateWebView(
			Width, Height, Session,
			Awesomium::kWebViewType_Offscreen
			);
	}
}

void InteractiveTV::Project::Oasis::Web::AppBase::DestroyCoreObjects()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Web::AppBase::DestroyCoreObjects()\n");

	if (View) View->Destroy(), View = nullptr;
	if (Session) Session->Release(), Session = nullptr;
}

InteractiveTV::Project::Oasis::Web *
InteractiveTV::Project::Oasis::Web::GetForCurrentThread()
{
	static InteractiveTV::Project::Oasis::Web s_web; return &s_web;
}

InteractiveTV::Project::Oasis::Web::Web()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Web::Web()\n");

	auto xsc = &Oasis::GetForCurrentThread()->Context;
	Oasis::String log_path = xsc->Path + ".osmlog.txt";

	m_core_configuration.log_level = Awesomium::kLogLevel_Verbose;
	m_core_configuration.log_path = Awesomium::WSLit(log_path.c_str());
	m_core_configuration.plugin_path = Awesomium::WSLit("C:/Windows/SysWOW64/Macromed/Flash/");
	//m_core_configuration.plugin_path = Awesomium::WSLit("C:/Windows/System32/Macromed/Flash/");
}

InteractiveTV::Project::Oasis::Web::~Web()
{ 
}

void InteractiveTV::Project::Oasis::Web::Init()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Web::Init()\n");

	//auto xsc = &Oasis::GetForCurrentThread()->Context;
	m_core = Awesomium::WebCore::Initialize(
		m_core_configuration
		);
}

void InteractiveTV::Project::Oasis::Web::Quit()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Web::Quit()\n");

	for (auto &kid : s_kids) delete kid, kid = nullptr; s_kids.clear();
	if (m_core) Awesomium::WebCore::Shutdown(), m_core = nullptr;
}

void InteractiveTV::Project::Oasis::Web::OnFrameMove()
{
	if (m_core) m_core->Update();
}



