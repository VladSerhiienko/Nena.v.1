#pragma once

/// From DirectX Toolkit

#include <mutex>
#include <memory>
#include <algorithm>

#include <wrl.h>
#include <Windows.h>

namespace Nena
{
	namespace Platform
	{
		// Helper for output debug tracing
		template <BOOL _Print = TRUE, size_t _BuffLen = 512>
		inline char *DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...)
		{
#ifdef _DEBUG
			static char buff[_BuffLen];

			va_list args;
			va_start(args, format);
			vsprintf_s(buff, format, args);
			if (_Print) 
				OutputDebugStringA(buff);
			return buff;
#else
			return NULL;
#endif
		}

		template <typename _Ty, size_t _Sz = sizeof _Ty> void ZeroValue(_Ty *data)
		{
			memset(data, 0, _Sz);
		}

		template <typename _Ty> void ZeroValue(_Ty *data, size_t sz)
		{
			memset(data, 0, sz);
		}

		struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
		typedef public std::unique_ptr<void, handle_closer> ScopedHandle;

		inline HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }
		template<class T> class ScopedObject : public Microsoft::WRL::ComPtr<T> {};


		// __declspec(align(16)) struct MyAlignedType : public AlignedNew<MyAlignedType>
		template<typename TDerived> struct AlignedNew
		{
			static void* operator new (size_t size)
			{
				const size_t alignment = __alignof(TDerived);

				static_assert(alignment > 8, 
					"AlignedNew is only useful for types with > 8 byte alignment. "
					"Did you forget a __declspec(align) on TDerived?"
					);

				void* ptr = _aligned_malloc(size, alignment);
				if (!ptr) throw std::bad_alloc();

				return ptr;
			}

			static void operator delete (void* ptr)
			{
				_aligned_free(ptr);
			}

			static void* operator new[](size_t size)
			{
				return operator new(size);
			}

			static void operator delete[](void* ptr)
			{
				operator delete(ptr);
			}
		};

		// Helper for lazily creating a D3D resource.
		template<typename T, typename TCreateFunc>
		static T* DemandCreate(Microsoft::WRL::ComPtr<T>& comPtr, std::mutex& mutex, TCreateFunc createFunc, ::HRESULT &handle)
		{
			T* result = comPtr.Get();

			// Double-checked lock pattern.
			MemoryBarrier();

			if (!result)
			{
				std::lock_guard<std::mutex> lock(mutex);

				result = comPtr.Get();

				if (!result)
				{
					// Create the new object.
					handle = createFunc(&result);
					MemoryBarrier();
					comPtr.Attach(result);
				}
			}

			return result;
		}
	}
}
