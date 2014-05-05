
#include "app.precompiled.h"
#include "itv.oasis.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_AIR_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_AIR_INCLUDED__

namespace InteractiveTV
{
	struct Oasis::Air
	{

#pragma region Typedefs

		typedef std::string String;
		typedef struct Message
		{
			typedef enum 
			{ 
				kWarning, //! some problems occured, but its ok in general
				kError, //! feature will be supported partially
				kCritical, //! feature will not be supported
				kFatal, //! unexpected error, shutting down...
				kInfo //! information for testing
			} Type;
			String msg;
			Type type;

			Message(String msg) 
				: type(kInfo)
				, msg(msg) 
			{
			}

			Message(String msg, Type type)
				: type(type)
				, msg(msg)
			{
			}
		} Item;

		typedef std::deque<Message> StringDeque;
		typedef StringDeque::size_type StringDequeSize;

#pragma endregion

		static Air::StringDequeSize const MaxMessageSize = 128;
		static Air::StringDequeSize const MaxMessageCount = 32;

#pragma region Static tracers

		template <size_t _BuffLen = 512, BOOL _Print = TRUE>
		static inline char *ConsoleTrace(_In_z_ _Printf_format_string_ const char* format, ...)
		{
			static char buff[_BuffLen];

			va_list args;
			va_start(args, format);
			vsprintf_s(buff, format, args);
			if (_Print) std::cout << buff << std::endl;
			return buff;
		}

		template <size_t _BuffLen = 512, BOOL _Print = TRUE>
		static inline wchar_t *ConsoleTrace(_In_z_ _Printf_format_string_ const wchar_t* format, ...)
		{
			static wchar_t buff[_BuffLen];

			va_list args;
			va_start(args, format);
			vswprintf_s(buff, format, args);
			if (_Print) std::wcout << buff << std::endl;
			return buff;
		}

		template <size_t _BuffLen = 512, BOOL _Print = TRUE>
		static inline char *DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...)
		{
#ifdef _DEBUG
			static char buff[_BuffLen];

			va_list args;
			va_start(args, format);
			vsprintf_s(buff, format, args);
			if (_Print) ::OutputDebugStringA(buff),
				::OutputDebugStringA("\n")
			return buff;
#else
			return NULL;
#endif
		}

		template <size_t _BuffLen = 512, BOOL _Print = TRUE>
		static inline wchar_t *DebugTrace(_In_z_ _Printf_format_string_ const wchar_t* format, ...)
		{
#ifdef _DEBUG
			static wchar_t buff[_BuffLen];

			va_list args;
			va_start(args, format);
			vswprintf_s(buff, format, args);
			if (_Print) ::OutputDebugStringW(buff),
				::OutputDebugStringA("\n")
			return buff;
#else
			return NULL;
#endif
		}

#pragma endregion

		Air( );
		~Air( );

		void Grab(Air::String);
		void Grab(Air::String, Air::Message::Type);
		void Grab(Air::Message::Type, Air::String);

		void Grab(Oasis::Object *, Air::String);
		void Grab(Oasis::Object *, Air::String, Air::Message::Type);
		void Grab(Oasis::Object *, Air::Message::Type, Air::String);
		Air::StringDeque::const_iterator Begin();
		Air::StringDeque::const_iterator End();
		Air::StringDeque::size_type Size();

		static Air *GetForCurrentThread();

	private:

		::BOOL is_available;
		::SRWLOCK msg_collection_guard;
		Air::StringDeque msg_collection;

	};

	typedef Oasis::Air::Message::Type OasisAirMsg;
}

#define _Oasis_air_fmt(...) InteractiveTV::Oasis::Air::ConsoleTrace<512, FALSE>(fmt, __VA_ARGS__)
#define _Oasis_air_grab(...) InteractiveTV::Oasis::Air::GetForCurrentThread()->Grab( \
	InteractiveTV::Oasis::Air::ConsoleTrace<512, FALSE>(__VA_ARGS__))
#define _Oasis_air_grabt(t, ...) InteractiveTV::Oasis::Air::GetForCurrentThread()->Grab( \
	t, InteractiveTV::Oasis::Air::ConsoleTrace<1024, FALSE>(__VA_ARGS__))
#define _Oasis_air_grabtc(t, ...) InteractiveTV::Oasis::Air::GetForCurrentThread()->Grab( \
	t, InteractiveTV::Oasis::Air::ConsoleTrace<1024, TRUE>(__VA_ARGS__))
#define _Oasis_air_grabot(o, t, ...) InteractiveTV::Oasis::Air::GetForCurrentThread()->Grab( \
	o, t, InteractiveTV::Oasis::Air::ConsoleTrace<1024, FALSE>(__VA_ARGS__))
#define _Oasis_air_grabotc(o, t, ...) InteractiveTV::Oasis::Air::GetForCurrentThread()->Grab( \
	o, t, InteractiveTV::Oasis::Air::ConsoleTrace<1024, TRUE>(__VA_ARGS__))

#endif // !__NENA_INTERACTIVE_TV_OASIS_AIR_INCLUDED__


