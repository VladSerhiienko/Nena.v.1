
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

#define _Nena_Ignore_DefaultImeW L"Default IME"
#define _Nena_Ignore_MsctfimeW L"MSCTFIME UI"

std::map<HWND, std::string> g_windows;
std::map<HWND, std::string> g_images;
std::map<HWND, std::string> g_classes;
std::map<HWND, std::string> g_windows2;
std::map<HWND, std::string> g_images2;
std::map<HWND, std::string> g_classes2;
std::vector<HWND> g_matched;

struct Target
{
	std::string Name;
	std::string Class;
	std::string Image;
};

std::vector<Target> g_targets;

BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam)
{
	WCHAR buffer[1024] = { L"\0" };
	INT32 len = GetWindowTextW(hWnd, buffer, ARRAYSIZE(buffer));
	if (!len || 
		(std::wstring(buffer) == _Nena_Ignore_DefaultImeW) ||
		(std::wstring(buffer) == _Nena_Ignore_MsctfimeW))
		return TRUE;

	std::string bufferconv;
	std::wstring bufferwconv = buffer;
	Nena::Converter::String16To8(bufferconv, bufferwconv);
	g_windows[hWnd] = bufferconv;

	GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer));
	bufferwconv = buffer;
	Nena::Converter::String16To8(bufferconv, bufferwconv);
	g_classes[hWnd] = bufferconv;

	DWORD processId;
	GetWindowThreadProcessId(hWnd, &processId);

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, processId);

	if (processHandle)
	{
		BOOL ok;
		DWORD size;

		size = GetModuleFileNameExW(processHandle, NULL, buffer, ARRAYSIZE(buffer));
		bufferwconv = buffer;
		Nena::Converter::String16To8(bufferconv, bufferwconv);
		g_images[hWnd] = bufferconv;
	}

	CloseHandle(processHandle);
	return TRUE;
}

void PrintToFiles()
{
	::EnumWindows(enumWindowsProc, NULL);

	std::string wnd;
	std::string wndname;

	std::ofstream names("window-names.txt");
	for (auto &p : g_windows)
	{
		Nena::Converter::ToString(wnd, (UINT32) p.first);
		wndname = p.second;

		names << wnd.c_str() << " " << wndname << "\n";

		OutputDebugStringA("-window: ");
		OutputDebugStringA(wndname.c_str());
		OutputDebugStringA("\n");

	}
	names.close();

	std::ofstream images("window-images.txt");
	for (auto &p : g_images)
	{
		Nena::Converter::ToString(wnd, (UINT32) p.first);
		wndname = p.second;

		images << wnd.c_str() << " " << wndname.c_str() << "\n";

		OutputDebugStringA("-image: ");
		OutputDebugStringA(wndname.c_str());
		OutputDebugStringA("\n");
	}
	images.close();

	std::ofstream classes("window-classes.txt");
	for (auto &p : g_classes)
	{
		Nena::Converter::ToString(wnd, (UINT32) p.first);
		wndname = p.second;

		classes << wnd.c_str() << " " << wndname.c_str() << "\n";

		OutputDebugStringA("-class: ");
		OutputDebugStringA(wndname.c_str());
		OutputDebugStringA("\n");
	}
	classes.close();
}

void SearchFromFiles()
{
	errno_t err;

	FILE *names; err = fopen_s(&names, "window-names.txt", "r+");
	FILE *images; err = fopen_s(&images, "window-images.txt", "r+");
	FILE *classes; err = fopen_s(&classes, "window-classes.txt", "r+");

	UINT32 id = 0;
	CHAR buffer[1024] = { 0 };

	while ((err = fscanf_s(names, "%u %1000[^\n]", &id, buffer, _countof(buffer))) != EOF)
	{
		g_windows2[(HWND) id] = buffer; 
		OutputDebugStringA("+window: ");
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
	}
	while ((err = fscanf_s(images, "%u %1000[^\n]", &id, buffer, _countof(buffer))) != EOF)
	{
		g_images2[(HWND) id] = buffer;
		OutputDebugStringA("+image: ");
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
	}
	while ((err = fscanf_s(classes, "%u %1000[^\n]", &id, buffer, _countof(buffer))) != EOF)
	{
		g_classes2[(HWND) id] = buffer;
		OutputDebugStringA("+class: ");
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
	}

	_fcloseall();

	EnumWindows(&::enumWindowsProc, 0);


}

int main(int argc, CHAR* argv[])
{
	int cmd = 1;
	if (argc <= 1) cmd = 1;
	else if (!strcmp(argv[1], "-print")) cmd = 1;
	else if (!strcmp(argv[1], "-search")) cmd = 2;
	else if (!strcmp(argv[1], "-focus")) cmd = 3;
	else if (!strcmp(argv[1], "-emission")) cmd = 4;

	cmd = 4;

	CHAR* ids[] = 
	{
		"", // app path
		"", // app cnfg
		"984628",
		//"1442526",
		//"2294590",
	};

	if (argc == 1)
	{
		argc = ARRAYSIZE(ids);
		argv = ids;
	}

	switch (cmd)
	{
	case 1: PrintToFiles(); break;
	case 2: SearchFromFiles(); break;
	case 3: 
		{
			PrintToFiles();
			SearchFromFiles();

			UINT32 targetCount = argc - 2;
			std::vector<HWND> targetsVector;
			for (UINT32 target = 0; target < targetCount; target++)
			{
				UINT32 targetId;
				std::string targetString = argv[target + 2];
				Nena::Converter::ToNumber(targetString, targetId);
				Nena::Platform::DebugTrace("%u\n", targetId);

				targetsVector.push_back((HWND) targetId);
			}

			Nena::Platform::DebugTrace("we would like to find:\n");
			Nena::Platform::DebugTrace("-----------------------------------\n");
			for (auto target : targetsVector)
				Nena::Platform::DebugTrace("name  %s\n", g_windows2[target].c_str()),
				Nena::Platform::DebugTrace("class %s\n", g_classes2[target].c_str()),
				Nena::Platform::DebugTrace("image %s\n", g_images2[target].c_str()),
				Nena::Platform::DebugTrace("-----------------------------------\n");

			UINT32 targetsFound = 0;
			for (auto target : targetsVector)
			if (g_windows.find(target) != g_windows.end() && (*g_windows.find(target)).second == g_windows2[target] &&
				g_classes.find(target) != g_classes.end() && (*g_classes.find(target)).second == g_classes2[target] &&
				g_images.find(target) != g_images.end() && (*g_images.find(target)).second == g_images2[target])
			{
				Nena::Platform::DebugTrace("valid window %u\n", target), targetsFound++;

				Target windowTarget;
				windowTarget.Class = (*g_classes.find(target)).second;
				windowTarget.Image = (*g_images.find(target)).second;
				windowTarget.Name = (*g_windows.find(target)).second;
				g_targets.push_back(windowTarget);
			}

			std::ofstream targetFile("window-targets.txt");
			for (auto target : g_targets)
				targetFile <<
					target.Name << "\n" <<
					target.Class << "\n" <<
					target.Image << "\n";
			targetFile.close();

		} break;
	case 4:
		{
			std::ifstream targetFile("window-targets.txt");
			while (targetFile.good())
			{
				Target target;
				CHAR buffer[128];

				targetFile.getline(buffer, _countof(buffer));
				if (!targetFile.good()) break;
				target.Name = buffer;
				targetFile.getline(buffer, _countof(buffer));
				if (!targetFile.good()) break;
				target.Class = buffer;
				targetFile.getline(buffer, _countof(buffer));
				if (!targetFile.good()) break;
				target.Image = buffer;

				g_targets.push_back(target);
			}
			targetFile.close();

			PrintToFiles();

			std::vector<HWND> targetWindows;
			std::vector<HWND> targetWindowsNi;
			std::vector<HWND> targetWindowsIc;
			std::vector<HWND> targetWindowNames;
			std::vector<HWND> targetWindowImages;
			std::vector<HWND> targetWindowClasses;

			for (auto pair : g_windows)
			for (auto &target : g_targets)
			if (target.Name == pair.second)
				targetWindowNames.push_back(pair.first);

			for (auto pair : g_classes)
			for (auto &target : g_targets)
			if (target.Class == pair.second)
				targetWindowClasses.push_back(pair.first);

			for (auto pair : g_images)
			for (auto &target : g_targets)
			if (target.Image == pair.second)
				targetWindowImages.push_back(pair.first);

			std::sort(targetWindowNames.begin(), targetWindowNames.end());
			std::sort(targetWindowImages.begin(), targetWindowImages.end());
			std::sort(targetWindowClasses.begin(), targetWindowClasses.end());

			std::set_intersection(
				targetWindowNames.begin(), targetWindowNames.end(), 
				targetWindowImages.begin(), targetWindowImages.end(), 
				std::back_inserter(targetWindowsNi)
				);

			std::set_intersection(
				targetWindowImages.begin(), targetWindowImages.end(), 
				targetWindowClasses.begin(), targetWindowClasses.end(), 
				std::back_inserter(targetWindowsIc)
				);

			std::sort(targetWindowsNi.begin(), targetWindowsNi.end());
			std::sort(targetWindowsIc.begin(), targetWindowsIc.end());

			std::set_intersection(
				targetWindowsNi.begin(), targetWindowsNi.end(),
				targetWindowsIc.begin(), targetWindowsIc.end(),
				std::back_inserter(targetWindows)
				);

		} break;
	default: 
		break;
	}

	std::cout << "Press any key to exit" << std::endl;
	std::cin.get();
	return EXIT_SUCCESS;
}