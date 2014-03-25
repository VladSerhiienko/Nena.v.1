
#define _Nena_Ignore_DefaultImeW L"Default IME"
#define _Nena_Ignore_MsctfimeW L"MSCTFIME UI"

#include "RemoteControl.h"
using namespace Demo;

HRESULT RemoteControl::WriteToplevelWindows(::std::string file)
{
	::EnumWindows((WNDENUMPROC)RemoteControl::SearchRoutine, (LPARAM) this);

	std::ofstream targetFile(file);
	for (auto &t : Targets)
	{
		targetFile << t.Name << "\n" << t.Class << "\n" << t.Image << "\n";
		targetFile << "-------------------------------------------------------" << "\n";
		//targetFile << "\"" << t.Name << "\"" << " " << "\"" << t.Class << "\"" << "\n";
		//targetFile << "\"" << t.Image << "\"\n";
		//targetFile << "-------------------------------------------------------" << "\n";
	}

	targetFile.close();
	return S_OK;
}

HRESULT RemoteControl::ReadFocusData(::std::string file)
{
	CHAR buffer[1024] = { 0 };

	std::ifstream targetFile(file);
	while (targetFile.good())
	{
		FocusData target;

		targetFile.getline(buffer, _countof(buffer));
		if (!targetFile.good()) break;
		target.Name = buffer;

		targetFile.getline(buffer, _countof(buffer));
		if (!targetFile.good()) break;
		target.Class = buffer;

		targetFile.getline(buffer, _countof(buffer));
		if (!targetFile.good()) break;
		target.Image = buffer;

		Targets.push_back(target);

	} targetFile.close();

	if (Targets.empty()) return E_FAIL;
	return S_OK;
}

HRESULT RemoteControl::UpdateFocusHandles()
{
	if (Targets.empty()) return E_INVALIDARG;
	else Focus.clear();

	for (auto &t : Targets)
	{
		HWND handle = FindWindowA(t.Class.c_str(), t.Name.c_str());
		if (IsWindow(handle)) Focus.push_back(handle);
	}

	if (Focus.empty()) return E_INVALIDARG;
	return S_OK;
}

void RemoteControl::SendMessageToTargets(UINT msg, WPARAM wp, LPARAM lp)
{
	BOOL result = TRUE;
	BOOL updateNeeded = FALSE;

	if (Focus.empty()) UpdateFocusHandles();

	for (auto &f : Focus)
	{
		result = PostMessageA(f, msg, wp, lp);
		if (!updateNeeded && !result)
			updateNeeded = TRUE;
	}
	
	if (updateNeeded) UpdateFocusHandles();
}

BOOL RemoteControl::SearchRoutine(HWND handle, LPARAM param)
{
	RemoteControl *controller = nullptr;

	if (!param) return FALSE;
	else controller = (RemoteControl *) param;
	if (!controller) return FALSE;

	WCHAR buffer[1024] = { L"\0" };
	RemoteControl::FocusData focus;
	focus.Handle = handle;

	INT32 len = GetWindowTextW(handle, buffer, ARRAYSIZE(buffer));
	if (!len || (std::wstring(buffer) == _Nena_Ignore_DefaultImeW) ||
		(std::wstring(buffer) == _Nena_Ignore_MsctfimeW)) return TRUE;
	std::string bufferconv;
	std::wstring bufferwconv = buffer;
	Nena::Converter::String16To8(bufferconv, bufferwconv);
	if (bufferconv.empty()) return TRUE; // continue
	else focus.Name = bufferconv;

	GetClassNameW(handle, buffer, ARRAYSIZE(buffer));
	std::string bufferconv2;
	std::wstring bufferwconv2 = buffer;
	Nena::Converter::String16To8(bufferconv2, bufferwconv2);
	if (bufferconv2.empty()) return TRUE; // continue
	else focus.Class = bufferconv2;

	DWORD processId;
	GetWindowThreadProcessId(handle, &processId);

	HANDLE processHandle = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION |
		PROCESS_VM_READ, FALSE,
		processId
		);

	if (processHandle && processHandle != INVALID_HANDLE_VALUE)
	{
		if (GetModuleFileNameExW(processHandle, NULL, buffer, ARRAYSIZE(buffer)))
		{
			std::string bufferconv3;
			std::wstring bufferwconv3 = buffer;
			Nena::Converter::String16To8(bufferconv3, bufferwconv3);
			focus.Image = bufferconv3;
		}
	}
	else focus.Image = "DEMO :: ACCESS DENIED :: DEMO";

	controller->Targets.push_back(focus);
	CloseHandle(processHandle);
	return TRUE;
}
