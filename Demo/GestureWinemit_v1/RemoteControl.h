
#pragma once

#include <conio.h>
#include <wchar.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <Windows.h>
#include <ppl.h>
#include <ppltasks.h>
#include <Psapi.h>

#include <Nena\AppVars.h>
#include <Nena\AppConverter.h>
#include <Nena\PlatformAssist.h>

namespace Demo
{

	struct RemoteControl
	{
		struct FocusData
		{
			::HWND Handle;
			::std::string Name;
			::std::string Class;
			::std::string Image;
		};

		HRESULT WriteToplevelWindows(::std::string file = "toplevelwindows.txt");
		HRESULT ReadFocusData(::std::string file = "focuswindows.txt");
		HRESULT UpdateFocusHandles();

		void SendMessageToTargets(UINT, WPARAM, LPARAM);

		static BOOL CALLBACK SearchRoutine(HWND, LPARAM);

		std::vector<FocusData> Targets;
		std::vector<HWND> Focus;
	};
}