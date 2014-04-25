#include "Engine.h"

#include <Nena\App.h>
#include <Nena\AppVars.h>
#include <Nena\PlatformAssist.h>

#include "ShadowMapping.h"

INT32 WINAPI WinMain(
	::HINSTANCE hInstance,
	::HINSTANCE hPrevInstance,
	::LPTSTR pScmdline,
	::INT32 iCmdshow
	)
{
	Nena::App *app = Nena::App::GetForCurrentThread();
	Demo::Engine *engine = Demo::Engine::GetForCurrentThread();

	Demo::ShadowMapping::Get()->UploadResources();
	Demo::ShadowMapping::Get()->CreateDeviceResources();

	app->MessageLoop();

	return 0;
}