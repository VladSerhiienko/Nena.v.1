#include "app.precompiled.h"
#include "itv.oasis.h"


#ifndef __NENA_BITMAP_HELPER_INCLUDED__
#define __NENA_BITMAP_HELPER_INCLUDED__

namespace Nena
{
	namespace Graphics
	{
		namespace Resources
		{
			struct BitmapHelp;
		}
	}
}

struct Nena::Graphics::Resources::BitmapHelp
{
	typedef ::std::string String8;
	typedef ::std::wstring String, String16;

	static Nena::Graphics::Resources::Direct2DBitmap LoadFromFile(
		_In_ BitmapHelp::String8 uri, _Out_opt_::HRESULT *result = nullptr
		);
	static Nena::Graphics::Resources::Direct2DBitmap LoadFromFile(
		_In_ BitmapHelp::String16 uri, _Out_opt_::HRESULT *result = nullptr
		);
};

#endif // !__NENA_BITMAP_HELPER_INCLUDED__
