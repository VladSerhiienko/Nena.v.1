#include "app.precompiled.h"
#include "itv.oasis.home.h"

struct InteractiveTV::Project::Oasis::Implementation 
	: public InteractiveTV::Project::Oasis::Object
{
	static ::INT16 s_next_id;

	Oasis::Home *home;

	void AssignId(Oasis::Object *obj)
	{
		obj->Id = s_next_id++;
	}

	Implementation() 
		: home(new Oasis::Home) 
	{ 
		OutputDebugStringA("InteractiveTV::Project::Oasis::Implementation::Implementation()\n");

		AssignId(this);
		Name = "/.//oasis/";
		home->Name = Name;
		home->Name += "home/";
	}

};

::INT16 InteractiveTV::Project::Oasis::Implementation::s_next_id = 0;

void InteractiveTV::Project::Oasis::AssignId(Oasis::Object *object)
{
	impl->AssignId(object);
}

void InteractiveTV::Project::Oasis::SetName(
	Oasis::Object *master, 
	Oasis::Object *slave, 
	String const &name
	)
{
	if (master && slave) slave->Name += master->Name + name + '/';
	else if (slave) slave->Name = name;
}

InteractiveTV::Project::Oasis::Shared::Shared()
{
	using namespace Nena;
	using namespace Nena::Application;

	OutputDebugStringA("InteractiveTV::Project::Oasis::Shared::Shared()\n");


	Oasis::GenerateUuid();
	Path = "./.oasis/{" + Uuid + "}/";
	Oasis::CreateFolder(Path);

	App = App::GetForCurrentThread();

	App->View.Fullscreen = TRUE;
	App->View.Width = 1200;
	App->View.Height = 800;
	App->View.Style = Window::GameWindowStyle;
	App->EnableCrtChecks();
	App->MissingInit(App);
	App->View.UserData = this;
	App->UserData = this;
	App->OnInit = NULL;

	Engine = Engine::GetForCurrentThread();
	Engine->Graphics.d3d.Windowed = !App->View.Fullscreen;

	Web = Web::GetForCurrentThread();
	Oasis = Oasis::GetForCurrentThread();

	Web->Init();
	Engine->Init();
}

InteractiveTV::Project::Oasis::Oasis()
	: impl(new Implementation)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Oasis()\n");

	Oasis::SetName(impl, &Context, "shared");

	Context.App->QuitRequested += Nena_Delegate_MakeBind(
		this, &Oasis::OnAppQuitRequested
		);
}

InteractiveTV::Project::Oasis::~Oasis()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::~Oasis()\n");
	if (impl) delete impl, impl = nullptr;
}

void InteractiveTV::Project::Oasis::Run()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Run()\n");
	impl->home->Init();

	Context.App->OnUpdate = NULL;
	Context.App->View.ResizedMoved += Nena_Delegate_MakeBind(this, &Oasis::OnViewResizedMoved);
	Context.App->View.Toggled += Nena_Delegate_MakeBind(this, &Oasis::OnViewResizedMoved);

	Context.App->MessageLoop();
}

InteractiveTV::Project::Oasis::State::State(Oasis::State *master)
	: Context(&Oasis::GetForCurrentThread()->Context)
	, Master(master)
{
	Context->Oasis->AssignId(this);
	if (master) Name = master->Name;

	Resumed += Nena_Delegate_MakeBind(Context->Oasis, &Oasis::OnStateResumed);
	Quitted += Nena_Delegate_MakeBind(Context->Oasis, &Oasis::OnStateQuitted);
	Suspended += Nena_Delegate_MakeBind(Context->Oasis, &Oasis::OnStateSuspended);
	Suspended += Nena_Delegate_MakeBind(Context->Oasis, &Oasis::OnStateDestroyed);

	if (master)
	{
		Quitted += Nena_Delegate_MakeBind(master, &Oasis::State::OnStateQuitted);
		Resumed += Nena_Delegate_MakeBind(master, &Oasis::State::OnStateResumed);
		Suspended += Nena_Delegate_MakeBind(master, &Oasis::State::OnStateSuspended);
	}
}

InteractiveTV::Project::Oasis::State::~State()
{
	Destoyed(this);
}

void InteractiveTV::Project::Oasis::OnFrameMove()
{
	Context.Web->OnFrameMove();

	Context.Engine->BeginScene();
	impl->home->OnFrameMove();
	Context.Engine->EndScene();
}

void _Nena_AppCallbackObjCallTy_ InteractiveTV::Project::Oasis::OnUpdate(::Nena::App *app)
{
	//static Oasis *s_oasis = Oasis::GetForCurrentThread(); return s_oasis->OnFrameMove();
}


void InteractiveTV::Project::Oasis::OnViewResizedMoved(
	_In_::Nena::Application::Window *window
	)
{
	Nena::Graphics::Resources::Size sz;
	sz.Width = window->Fullscreen ? window->ScreenWidth : window->Width;
	sz.Height = window->Fullscreen ? window->ScreenHeight : window->Height;
	Context.Engine->Graphics.d3d.Windowed = !window->Fullscreen;

	Context.Engine->Graphics.d3d.SetLogicalSize(sz);
	Context.Engine->Graphics.d2d.CreateWindowSizeDependentResources(
		Context.Engine->Graphics.d3d.Swapchain.Get()
		);

	impl->home->OnViewResizedMoved(window);
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
	OutputDebugStringA("InteractiveTV::Project::Oasis::OnStateDestroyed()\n");
	OutputDebugStringA("\t"); OutputDebugStringA(state->GetNameCcstr()); OutputDebugStringA("\n");
}

#pragma endregion

void InteractiveTV::Project::Oasis::Quit()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Quit()\n");

	OnQuit();
	::PostQuitMessage(EXIT_SUCCESS);
}
void InteractiveTV::Project::Oasis::OnQuit()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::OnQuit()\n");

	Context.Web->Quit();
	Context.Engine->Quit();
}
void InteractiveTV::Project::Oasis::OnAppQuitRequested(::Nena::App *app)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::OnAppQuitRequested()\n");

	OnQuit();
}

void InteractiveTV::Project::Oasis::OnKeyPressed(::UINT32 key)
{
	Awesomium::WebKeyboardEvent e;
	ZeroMemory(&e, sizeof e);
	e.type = Awesomium::WebKeyboardEvent::kTypeKeyDown;
	e.virtual_key_code = key;
	//e.native_key_code = key;

	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectKeyboardEvent(e);
}

void InteractiveTV::Project::Oasis::OnKeyReleased(::UINT32 key)
{
	Awesomium::WebKeyboardEvent e;
	ZeroMemory(&e, sizeof e);
	e.type = Awesomium::WebKeyboardEvent::kTypeKeyUp;
	e.virtual_key_code = key;
	//e.native_key_code = key;

	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectKeyboardEvent(e);
}

void InteractiveTV::Project::Oasis::OnMouseMoved(::UINT32 x, ::UINT32 y)
{
	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectMouseMove(x, y);
}

void InteractiveTV::Project::Oasis::OnMouseLBPressed(::UINT32 x, ::UINT32 y)
{
	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
}

void InteractiveTV::Project::Oasis::OnMouseRBPressed(::UINT32 x, ::UINT32 y)
{
	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Right);
}

void InteractiveTV::Project::Oasis::OnMouseLBReleased(::UINT32 x, ::UINT32 y)
{
	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);
}

void InteractiveTV::Project::Oasis::OnMouseRBReleased(::UINT32 x, ::UINT32 y)
{
	if (!impl->home->Oasis::Web::AppBase::View->IsLoading())
		impl->home->Oasis::Web::AppBase::View->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);
}

#pragma region Utility

void InteractiveTV::Project::Oasis::GenerateUuid(
	)
{
	::UUID uuid;
	::RPC_CSTR rpc_cstring;
	::RPC_STATUS rpc_status;
	::CHAR uuid_buffer[MAX_PATH];

	rpc_status = ::UuidCreate(&uuid);
	rpc_status = ::UuidToStringA(&uuid, &rpc_cstring);

	sprintf_s(uuid_buffer, "%s", rpc_cstring);

	Oasis::GetForCurrentThread()->Context.Uuid = uuid_buffer;

	::RpcStringFreeA(&rpc_cstring);
}

::BOOL InteractiveTV::Project::Oasis::CreateFolder(
	Oasis::String directory
	)
{
	::DWORD fileAttributes = ::GetFileAttributesA(directory.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		String::size_type slashIndex = directory.find_last_of("\\/");
		if (slashIndex != String::npos) CreateFolder(directory.substr(0, slashIndex));
		return ::CreateDirectoryA(directory.c_str(), nullptr);
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
