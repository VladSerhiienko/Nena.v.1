
#include <Windows.h>
#include <string>
#include <sal.h>

#ifndef __NENA_APPCONVERTER_INCLUDED__
#define __NENA_APPCONVERTER_INCLUDED__

namespace Nena
{
	namespace Converter
	{
		inline void String8To16(_In_z_ const std::string &szValue8, _Outref_ std::wstring &szValue16) { szValue16 = std::wstring(szValue8.begin(), szValue8.end()); }
		inline void String16To8(_Outref_ std::string &szValue8, _In_z_ const std::wstring &szValue16) { szValue8 = std::string(szValue16.begin(), szValue16.end()); }
		inline std::wstring String8To16(_In_z_ const std::string &szValue8) { return std::wstring(szValue8.begin(), szValue8.end()); }
		inline std::string String16To8(_In_z_ const std::wstring &szValue16) { return std::string(szValue16.begin(), szValue16.end()); }

		inline void ToNumber(_In_z_ const std::wstring &szValue16, _Outref_ DOUBLE &fValue64) { fValue64 = std::stod(szValue16.c_str()); }
		inline void ToNumber(_In_z_ const std::wstring &szValue16, _Outref_ FLOAT &fValue32) { fValue32 = std::stof(szValue16.c_str()); }
		inline void ToNumber(_In_z_ const std::wstring &szValue16, _Outref_ UINT64 &iValue64) { iValue64 = std::stoull(szValue16.c_str()); }
		inline void ToNumber(_In_z_ const std::wstring &szValue16, _Outref_ UINT32 &iValue32) { iValue32 = std::stoul(szValue16.c_str()); }
		inline void ToNumber(_In_z_ const std::wstring &szValue16, _Outref_ INT64 &iValue64) { iValue64 = std::stoll(szValue16.c_str()); }
		inline void ToNumber(_In_z_ const std::wstring &szValue16, _Outref_ INT32 &iValue32) { iValue32 = std::stoi(szValue16.c_str()); }

		inline void ToNumber(_In_z_ const std::string &szValue8, _Outref_ DOUBLE &fValue64) { fValue64 = std::stod(szValue8.c_str()); }
		inline void ToNumber(_In_z_ const std::string &szValue8, _Outref_ FLOAT &fValue32) { fValue32 = std::stof(szValue8.c_str()); }
		inline void ToNumber(_In_z_ const std::string &szValue8, _Outref_ UINT64 &iValue64) { iValue64 = std::stoull(szValue8.c_str()); }
		inline void ToNumber(_In_z_ const std::string &szValue8, _Outref_ UINT32 &iValue32) { iValue32 = std::stoul(szValue8.c_str()); }
		inline void ToNumber(_In_z_ const std::string &szValue8, _Outref_ INT64 &iValue64) { iValue64 = std::stoll(szValue8.c_str()); }
		inline void ToNumber(_In_z_ const std::string &szValue8, _Outref_ INT32 &iValue32) { iValue32 = std::stoi(szValue8.c_str()); }

		inline void ToString(_Outref_ std::wstring &szValue16, _In_ const DOUBLE &fValue64) { szValue16 = std::to_wstring(fValue64); }
		inline void ToString(_Outref_ std::wstring &szValue16, _In_ const FLOAT &fValue32) { szValue16 = std::to_wstring(fValue32); }
		inline void ToString(_Outref_ std::wstring &szValue16, _In_ const INT64 &iValue64) { szValue16 = std::to_wstring(iValue64); }
		inline void ToString(_Outref_ std::wstring &szValue16, _In_ const INT32 &iValue32) { szValue16 = std::to_wstring(iValue32); }
		inline void ToString(_Outref_ std::wstring &szValue16, _In_ const UINT64 &uiValue64) { szValue16 = std::to_wstring(uiValue64); }
		inline void ToString(_Outref_ std::wstring &szValue16, _In_ const UINT32 &uiValue32) { szValue16 = std::to_wstring(uiValue32); }

		inline void ToString(_Outref_ std::string &szValue16, _In_ const DOUBLE &fValue64) { szValue16 = std::to_string(fValue64); }
		inline void ToString(_Outref_ std::string &szValue16, _In_ const FLOAT &fValue32) { szValue16 = std::to_string(fValue32); }
		inline void ToString(_Outref_ std::string &szValue16, _In_ const INT64 &iValue64) { szValue16 = std::to_string(iValue64); }
		inline void ToString(_Outref_ std::string &szValue16, _In_ const INT32 &iValue32) { szValue16 = std::to_string(iValue32); }
		inline void ToString(_Outref_ std::string &szValue16, _In_ const UINT64 &uiValue64) { szValue16 = std::to_string(uiValue64); }
		inline void ToString(_Outref_ std::string &szValue16, _In_ const UINT32 &uiValue32) { szValue16 = std::to_string(uiValue32); }
	};
};

#endif // !__NENA_APPCONVERTER_INCLUDED__
