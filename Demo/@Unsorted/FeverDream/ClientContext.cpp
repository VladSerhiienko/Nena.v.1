#include "pch.h"
#include "ClientContext.h"

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;

ClientContext::ClientContext(
	_In_ Windows::Networking::Sockets::DatagramSocket^ socket
	) : m_socket(socket), m_connected(false)
{
}

ClientContext::~ClientContext()
{
	Safe_Delete(m_socket);
	Safe_Delete(m_writer);
}

void ClientContext::OnMessage(
	_In_ Windows::Networking::Sockets::DatagramSocket^ socket, 
	_In_ Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments
	)
{
	try
    {
		auto input = ref new ClientContextInput();
		auto reader = eventArguments->GetDataReader();
        auto stringLength = reader->UnconsumedBufferLength;
		auto numEntries = reader->ReadInt32();

		input->Entries = ref new Platform::Array<ClientContextInputEntry>(
			numEntries
			);

		for each ( auto &var in input->Entries )
		{
			ClientContextInputEntry entry;
			entry.XPosition = reader->ReadSingle();
			entry.YPosition = reader->ReadSingle();
			entry.Orientation = reader->ReadSingle();

			var = entry;
		}

		MessageReceived(this, input);

		Platform::String^ messageContent = "Received data from remote peer:\n";

		for each ( auto &entry in input->Entries )
		{
			messageContent += "Entry:\t";
			messageContent += entry.XPosition.ToString() + "\t";
			messageContent += entry.YPosition.ToString() + "\t";
			messageContent += entry.Orientation.ToString() + "\n";
		}

		PrintToDebugWindowOnly16(
			messageContent->Data()
			);
    }
    catch (Exception^ exception)
    {
		SocketErrorStatus socketError = SocketError::GetStatus(exception->HResult);
        if (socketError == SocketErrorStatus::ConnectionResetByPeer)
        {
            // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
            PrintToDebugWindowOnly16(L"Peer does not listen on the specific port.");
        }
        else if (socketError != SocketErrorStatus::Unknown)
        {
			Platform::String^ error = "Error happened when receiving a datagram: " + socketError.ToString();
            PrintToDebugWindowOnly16(error->Data());
        }
        else
        {
			Platform::String^ error = "Unexpected error when receiving a datagram: " + socketError.ToString();
            PrintToDebugWindowOnly16(error->Data());
            throw;
        }
	}
}

Windows::Storage::Streams::DataWriter^ ClientContext::GetWriter()
{
	if (m_writer == nullptr)
	{
		m_writer = ref new Windows::Storage::Streams::DataWriter(
			m_socket->OutputStream
			);
	}

	return m_writer;
}

boolean ClientContext::IsConnected()
{
	return m_connected;
}

void ClientContext::SetConnected()
{
	m_connected = true;
	ConnectionChanged(this, m_connected);
}

void ClientContext::OpenConnection( 
	_In_ Platform::String^ host, 
	_In_ Platform::String^ port
	)
{
	if (port == nullptr)
	{
		PrintToDebugWindowOnly8(
			"Service name is not provided."
			);
        return;
	}

	HostName^ hostName;
	try
	{
        hostName = ref new HostName(host);
    }
    catch (InvalidArgumentException^ e)
    {
		PrintToDebugWindowOnly8(
			"Invalid host name."
			);
        return;
    }

	if (CoreApplication::Properties->HasKey(ClientPrivateKey))
    {
		PrintToDebugWindowOnly8(
			"Client is already created."
			);
        return;
    }

    DatagramSocket^ socket = ref new DatagramSocket();
    ClientContext^ socketContext = ref new ClientContext(socket);
    socket->MessageReceived += ref new ClientContext::DatagramSocketMessageReceivedEvent(
		socketContext, &ClientContext::OnMessage
		);

	{
		Platform::String^ message = 
			"Connecting to: Host=" + host + 
			" Service=" + port + ".";
		PrintToDebugWindowOnly16(
			message->Data()
			);
	}

	CoreApplication::Properties->Insert(
		ClientPrivateKey, 
		socketContext
		);

    ClientContext::ActionAsync(socket->ConnectAsync(hostName, port)).
		then([socketContext] (ClientContext::ActionAsync previousTask)
    {
        try
        {
            // Try getting an exception.
            previousTask.get();
			PrintToDebugWindowOnly8(
				"Connected."
				);

            socketContext->SetConnected();
        }
        catch (Exception^ exception)
        {
           Platform::String^ message = 
				"Connection failed:" + 
				exception->Message;
			PrintToDebugWindowOnly16(
				message->Data()
				);

			socketContext->ConnectionChanged(
				socketContext, socketContext->m_connected
				);
        }
    });
}

ClientContext^ ClientContext::Get()
{
	ClientContext^ socketContext = dynamic_cast<ClientContext^>(
		CoreApplication::Properties->Lookup(ClientPrivateKey)
		);
	return socketContext;
}

void ClientContext::CloseConnection(
	)
{
	if (CoreApplication::Properties->HasKey(ClientPrivateKey))
		CoreApplication::Properties->Remove(ClientPrivateKey);
}

void ClientContext::SendMessage(
	ClientContextOutput output
	)
{
	auto socketContext = Get();

	if (socketContext == nullptr)
	{
		PrintToDebugWindowOnly8(
			"Null context."
			);
		return;
	}

	try
	{
		socketContext->GetWriter()->WriteInt32(output.Content);
		socketContext->GetWriter()->WriteSingle(output.XPosition);
		socketContext->GetWriter()->WriteSingle(output.YPosition);
		socketContext->GetWriter()->WriteSingle(output.Orientation);
		socketContext->GetWriter()->WriteSingle(output.KeyMask);
	}
	catch (Exception^ exception)
	{
		PrintToDebugWindowOnly8(
			"Failed to send message (1)."
			);
		PrintToDebugWindowOnly16(
			exception->Message->Data()
			);
	}

	ClientContext::UInt32Async(socketContext->GetWriter()->StoreAsync()).
		then([output] (ClientContext::UInt32Async writeTask)
	{
		try
		{
			// Try getting an excpetion.
			writeTask.get();

			PrintToDebugWindowOnly8("Sending....");
			/*PrintToDebugWindowOnly16(output.Content.ToString()->Data());
			PrintToDebugWindowOnly16(output.XPosition.ToString()->Data());
			PrintToDebugWindowOnly16(output.YPosition.ToString()->Data());
			PrintToDebugWindowOnly16(output.Orientation.ToString()->Data());
			PrintToDebugWindowOnly16(output.KeyMask.ToString()->Data());*/
		}
		catch (Exception^ exception)
		{
			PrintToDebugWindowOnly8("Failed to send message (2).");
			PrintToDebugWindowOnly16(exception->Message->Data());
		}
	});
}
