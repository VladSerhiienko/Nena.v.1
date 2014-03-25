#include "Framework.h"

namespace Framework
{
	namespace Application
	{

#if __NENA_WINDOW_Debug__ // Debugging

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

		static void Preconfigure()
		{
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
		}

#else // Releasing

static void Preconfigure()
{
}

#endif

	}
}

INT32 WINAPI WinMain(
	::HINSTANCE hInstance, 
	::HINSTANCE hPrevInstance, 
	::LPTSTR pScmdline, 
	::INT32 iCmdshow
	)
{
	Framework::Application::Preconfigure();
	Framework::Application::Start();
	Framework::Application::Stop();
}