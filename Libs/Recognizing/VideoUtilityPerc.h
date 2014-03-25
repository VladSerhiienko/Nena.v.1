#include "VideoPrecompPerc.h"

#ifndef __NENA_VIDEOUTILITYPERC_INCLUDED__
#define __NENA_VIDEOUTILITYPERC_INCLUDED__

namespace Nena
{
	namespace Video
	{
		struct Utility
		{
			typedef void Action;
			typedef HRESULT Result;
			typedef pxcStatus Status;

			template <typename _Ty>  inline static BOOL Failed(_Ty result) { return ((INT32) result) < 0; }
			template <typename _Ty>  inline static BOOL Succeeded(_Ty result) { return ((INT32) result) == 0; }
			template <> inline static BOOL Failed<INT32>(INT32 result) { return result < 0; }
			template <> inline static BOOL Failed<pxcStatus>(pxcStatus result) { return result < 0; }
			template <> inline static BOOL Failed<HRESULT>(HRESULT result) { return FAILED(result); }
			template <> inline static BOOL Succeeded<INT32>(INT32 result) { return result == 0; }
			template <> inline static BOOL Succeeded<pxcStatus>(pxcStatus result) { return result == 0; }
			template <> inline static BOOL Succeeded<HRESULT>(HRESULT result) { return SUCCEEDED(result); }
			template <typename _Ty>  static void GenericZeroMemory(_Ty *memory, size_t size = sizeof _Ty) { if (memory) ZeroMemory(memory, size); }
			template <typename _TyChar, UINT32 _NuChar> static UINT32 CharArraySize(_TyChar(&string)[_NuChar]) { return _NuChar; }
			template <typename _TyChar, UINT32 _NuChar> static errno_t CharArrayCopy(_TyChar(&to)[_NuChar], _TyChar *from) { return wcscpy_s<_NuChar>(to, from); }
			template <typename _TyFlagSrc, typename _TyFlagChk> static BOOL Checked(_TyFlagSrc src, _TyFlagChk chk) { return ((UINT32) src & (UINT32) chk) == (UINT32) chk; }
			template <typename _TyFlagSrc, typename _TyFlagChk> static BOOL Equals(_TyFlagSrc src, _TyFlagChk chk) { return (UINT32) src == (UINT32) chk; }

			static UINT32 GetStreamCountFromDesc(PXCCapture::VideoStream::DataDesc &desc);
			static UINT32 GetStreamCountFromDesc(PXCCapture::VideoStream::DataDesc *desc);

			template <typename _TyStatus = Status> struct BasicStatus
			{
				_TyStatus LastStatus;

				inline BOOL IsValid(void) { return Succeeded(); }
				inline _TyStatus &Check(void) { return LastStatus; }
				inline _TyStatus const &Check(void) const { return LastStatus; }
				inline BOOL Failed(void) { return Utility::Failed(LastStatus); }
				inline BOOL Succeeded(void) { return Utility::Succeeded(LastStatus); }

				BasicStatus(_TyStatus init = (_TyStatus) 0) : LastStatus(init) {}
			};
		};
	}
}

#endif // !__NENA_VIDEOUTILITYPERC_INCLUDED__
