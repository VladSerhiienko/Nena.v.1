#include "Engine.h"
#include "Fire.h"

#include <Nena\App.h>
#include <Nena\AppVars.h>
#include <Nena\PlatformAssist.h>

INT32 WINAPI WinMain(
	::HINSTANCE hInstance,
	::HINSTANCE hPrevInstance,
	::LPTSTR pScmdline,
	::INT32 iCmdshow
	)
{
	Nena::App *app = Nena::App::GetForCurrentThread();
	Demo::Engine *engine = Demo::Engine::GetForCurrentThread();

	Demo::FeuerFrei fire;
	engine->Fire = &fire;
	app->MessageLoop();

	return 0;
}