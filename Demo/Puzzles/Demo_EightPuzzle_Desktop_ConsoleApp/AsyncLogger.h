#pragma once

#include "Pch.h"
#include <ostream>
#include <fstream>

namespace Utility
{
	class Logger
	{
		typedef concurrency::concurrent_queue<std::string> MessageQueue;
		typedef concurrency::task<void> Cycle;
		typedef std::ofstream FileWriter;
		MessageQueue qMsg;
		FileWriter Writer;
		Cycle Routine;

	public:

		Logger( std::string szFile = "8PzzlOutput.txt" );
		~Logger();

		void PrintAllIfAny();

		void GrabWithArgs(  
			_Printf_format_string_ char const *wszFormat, // Message format
			_Printf_format_string_params_(ppArgs) ... // Message args
			);
		void Grab( 
			_In_ std::string szMsg // Message
			);
		void Grab(
			);
	};
};