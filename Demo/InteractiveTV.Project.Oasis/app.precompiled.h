#ifndef __NENA_INTERACTIVE_TV_PCH_INCLUDED__
#define __NENA_INTERACTIVE_TV_PCH_INCLUDED__

#include <Windows.h>
#include <debugapi.h>

#include <cctype>
#include <ctime>
#include <iostream>
#include <iomanip> 
#include <sstream>
#include <fstream>
#include <string>

#include <unordered_set>
#include <vector>
#include <set>
#include <map>

#include <Awesomium/Platform.h>
#include <Awesomium/WebCore.h>
#include <Awesomium/WebView.h>
#include <Awesomium/BitmapSurface.h>
#pragma warning (disable: 4267)
#include <Awesomium/STLHelpers.h>
#pragma warning (default: 4267)

#include <Nena/App.h>
#include <Nena/Math.h>
#include <Nena/AppVars.h>
#include <Nena/AppNeon.h>
#include <Nena/PlatformAssist.h>
#include <Nena/WindowAssets.h>
#include <Nena/Assist.h>
#include <Nena/DeviceResources.h>
#include <Nena/RenderTargetOverlay.h>

#pragma comment(lib, "Rpcrt4.lib")

#define __NENA_INCLUDE_FAST_DELEGATES__ TRUE
#define __NENA_INCLUDE_SR_DELEGATES__ TRUE

#include <Nena/Delegate.h>

namespace InteractiveTV
{
	namespace Project
	{
		class Oasis;
	}
}

#endif // !__NENA_INTERACTIVE_TV_PCH_INCLUDED__
