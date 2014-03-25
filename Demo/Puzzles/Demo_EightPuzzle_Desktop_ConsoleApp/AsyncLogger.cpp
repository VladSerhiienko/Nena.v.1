#include "Pch.h"
#include "AsyncLogger.h"


Utility::Logger::Logger( std::string szFile )
{
	Writer.open(szFile);
	_ASSERT_EXPR(Writer.is_open(), "File not found.");

	Routine = concurrency::create_task([](){});
}

Utility::Logger::~Logger()
{
	PrintAllIfAny();
	Routine.wait();
	Writer.close();
}

void Utility::Logger::Grab()
{
	qMsg.push("------------------------------\n");
}

void Utility::Logger::Grab( _In_ std::string szMsg )
{
	qMsg.push(szMsg);
}

void Utility::Logger::GrabWithArgs( 
	_Printf_format_string_ char const *wszFormat, /* Message format */ 
	_Printf_format_string_params_(ppArgs) ... /* Message args */ 
	)
{
	// Msg buffer.
	char aMsg16[512];
	// Fill the msg buffer out.
	va_list szArgs16; va_start( szArgs16, wszFormat );
	vsprintf_s( aMsg16, wszFormat, szArgs16 );
	va_end( szArgs16 ); // free args 

	// Append compiled message to the queue. 
	Grab( aMsg16 );
}

void Utility::Logger::PrintAllIfAny()
{
	Writer.flush();

	while (!qMsg.empty())
	{
		std::string szMsg;
		if (qMsg.try_pop(szMsg))
		{
			OutputDebugStringA(szMsg.data());
			Writer << szMsg.data();
		}
	}
}
