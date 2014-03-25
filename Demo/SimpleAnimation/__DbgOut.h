#pragma once

#include <stdio.h>
#include <Windows.h>

namespace Framework 
{ 
	template <unsigned _Len = 1024>
	void Output(const char* szFormat, ...)
	{
		static char szBuff[_Len];

		va_list arg; 
		va_start(arg, szFormat);
		_vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);

		OutputDebugStringA(szBuff);
	}
	template <unsigned _Len = 1024>
	void Output(const wchar_t* szFormat, ...)
	{
		static wchar_t wszBuff[_Len];

		va_list arg;
		va_start(arg, szFormat);
		_vswnprintf(wszBuff, sizeof(wszBuff), szFormat, arg);
		va_end(arg);

		OutputDebugStringW(wszBuff);
	}
}
