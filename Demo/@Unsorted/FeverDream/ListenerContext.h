#pragma once

#define ListenerPrivateKey "ListenerContextKey"

#include "pch.h"

using namespace Platform;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;


[Windows::Foundation::Metadata::WebHostHidden]
ref class ListenerContext sealed
{
	typedef CRITICAL_SECTION CriticalSectionDescriptor;
	typedef concurrency::task<IOutputStream^> OutputStreamAsync;
	typedef Windows::Foundation::TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^> DatagramSocketMessageReceivedEvent;
	typedef concurrency::task<void> ActionAsync;
	typedef concurrency::task<uint32> UInt32Async;

public:

	ListenerContext(
		_In_ Windows::Networking::Sockets::DatagramSocket^ listener
		);
	void OnMessageReceived(
		_In_ Windows::Networking::Sockets::DatagramSocket^ socket, 
		_In_ Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments
		);
    boolean IsMatching(
		_In_ Windows::Networking::HostName^ hostName, 
		_In_ Platform::String^ port
		);

	static ListenerContext^ Get();
	static void Close();
	static void Open(
		_In_ Platform::String^ service
		);

private:
	~ListenerContext();
	void EchoMessage(
		_In_ Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments
		);

	CriticalSectionDescriptor m_lock;

	Windows::Networking::Sockets::DatagramSocket^ m_listener;
	Windows::Storage::Streams::IOutputStream^ m_outputStream;
	Windows::Networking::HostName^ m_hostName;
	Platform::String^ m_port;

};