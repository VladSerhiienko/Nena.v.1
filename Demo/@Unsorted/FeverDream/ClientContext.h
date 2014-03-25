#pragma once

#include "pch.h"

#define ClientPrivateKey "ClientContextKey"

using namespace Platform;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;

value struct ClientContextInputEntry
{
	float32 XPosition;
	float32 YPosition;
	float32 Orientation;
};

value struct ClientContextOutput
{
	int32 Content;
	float32 XPosition;
	float32 YPosition;
	float32 Orientation;
	float32 KeyMask;
};

ref class ClientContextInput
{
	Platform::Array<ClientContextInputEntry>^ m_entries;

internal:

	property Platform::Array<ClientContextInputEntry>^ Entries
	{
		Platform::Array<ClientContextInputEntry>^ get()
		{
			return m_entries;
		}

		void set(Platform::Array<ClientContextInputEntry>^ value)
		{
			m_entries = value;
		}
	}
};

ref class ClientContext;
delegate void ClientContextInputCallback(
	_In_ ClientContext^ sender, 
	_In_ ClientContextInput^ eventArguments
	);
delegate void ClientContextConnectionCallback(
	_In_ ClientContext^ sender, 
	_In_ boolean connected
	);

[Windows::Foundation::Metadata::WebHostHidden]
ref class ClientContext sealed
{
	typedef Windows::Foundation::TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^> DatagramSocketMessageReceivedEvent;
	typedef concurrency::task<void> ActionAsync;
	typedef concurrency::task<uint32> UInt32Async;

public:

	event ClientContextInputCallback^ MessageReceived;
	event ClientContextConnectionCallback^ ConnectionChanged;

public:
	ClientContext(
		_In_ Windows::Networking::Sockets::DatagramSocket^ socket
		);

	void OnMessage(
		_In_ Windows::Networking::Sockets::DatagramSocket^ socket, 
		_In_ Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments
		);
	Windows::Storage::Streams:: DataWriter^ GetWriter(
		);

	static void OpenConnection( 
		_In_ Platform::String^ host, 
		_In_ Platform::String^ port
		);
	static void SendMessage(
		ClientContextOutput input
		);
	static void CloseConnection(
		);
	static ClientContext^ Get();

	boolean IsConnected(
		);
	void SetConnected(
		);

	
private:
	~ClientContext();

    Windows::Networking::Sockets::DatagramSocket^ m_socket;
    Windows::Storage::Streams::DataWriter^ m_writer;
    boolean m_connected;

};