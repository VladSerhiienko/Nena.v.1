#include "pch.h"
#include "ListenerContext.h"

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;


ListenerContext::ListenerContext(
		_In_ DatagramSocket^ listener
		) : m_listener(listener)
{
	_ASSERT_EXPR(
		listener != nullptr, 
		"listener != null"
		);
	InitializeCriticalSectionEx(
		&m_lock, 0u, 
		NoFlags
		);
}

void ListenerContext::EchoMessage(
	_In_ DatagramSocketMessageReceivedEventArgs^ eventArguments
	)
{
	if (!IsMatching(eventArguments->RemoteAddress, eventArguments->RemotePort))
    {
		Platform::String^ warning = "Got datagram from " + eventArguments->RemoteAddress->DisplayName + ":" +
            eventArguments->RemotePort + ", but already 'connected' to " + m_hostName->DisplayName + ":" +
            m_port;
		PrintToDebugWindowOnly16(
			warning->Data()
			);
	}

	ListenerContext::UInt32Async(m_outputStream->WriteAsync(eventArguments->GetDataReader()->DetachBuffer())).
		then([this] (ListenerContext::UInt32Async writeTask)
    {
        try
        {
            // Try getting an exception.
            writeTask.get();
        }
        catch (Exception^ exception)
        {
			PrintToDebugWindowOnly16(
				L"Echoing a message failed with error: "
				);
			PrintToDebugWindowOnly16(
				exception->Message->Data()
				);
        }
    });
}

void ListenerContext::OnMessageReceived(
	_In_ DatagramSocket^ socket, 
	_In_ DatagramSocketMessageReceivedEventArgs^ eventArguments
	)
{
	if (m_outputStream != nullptr) EchoMessage(eventArguments);
	else ListenerContext::OutputStreamAsync(socket->GetOutputStreamAsync(eventArguments->RemoteAddress, eventArguments->RemotePort)).
		then([this, socket, eventArguments] (IOutputStream^ stream)
	{
		EnterCriticalSection(&m_lock); 
		if (m_outputStream == nullptr)
		{
			m_outputStream = stream;
			m_hostName = eventArguments->RemoteAddress;
			m_port = eventArguments->RemotePort;
		} 

		LeaveCriticalSection(&m_lock);
	}).then( [/*this, socket, eventArguments*/] (ListenerContext::ActionAsync previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ exception)
        {
			PrintToDebugWindowOnly16(
				exception->Message->Data()
				);
        }
    });
}

boolean ListenerContext::IsMatching(
	_In_ Windows::Networking::HostName^ hostName, 
	_In_ Platform::String^ port
	)
{
	const boolean result = m_hostName == hostName && m_port == port;
	return result;
}

ListenerContext::~ListenerContext()
{
	Safe_Delete(m_listener);
	DeleteCriticalSection(&m_lock);
}

ListenerContext^ ListenerContext::Get()
{
	ListenerContext^ socketContext = dynamic_cast<ListenerContext^>(
		CoreApplication::Properties->Lookup(ListenerPrivateKey)
		);

	return socketContext;
}

void ListenerContext::Close()
{
	if (CoreApplication::Properties->HasKey(ListenerPrivateKey))
		CoreApplication::Properties->Remove(ListenerPrivateKey);
}

void ListenerContext::Open(
	_In_ Platform::String^ service
	)
{
	if (CoreApplication::Properties->HasKey(ListenerPrivateKey))
	{
		PrintToDebugWindowOnly8(
			"Already listening..."
			);
	}
	else
	{
		DatagramSocket^ listener = ref new DatagramSocket();
		ListenerContext^ listenerContext = ref new ListenerContext(listener);
		listener->MessageReceived += ref new DatagramSocketMessageReceivedEvent(
			listenerContext, &ListenerContext::OnMessageReceived
			);

		CoreApplication::Properties->Insert(
			ListenerPrivateKey, listenerContext
			);	

		ListenerContext::ActionAsync(listener->BindServiceNameAsync(service)).
			then( [/*this*/](ListenerContext::ActionAsync previousTask)
		{
			 try
			{
				previousTask.get();
			}
			catch (Exception^ exception)
			{
				CoreApplication::Properties->Remove(
					ListenerPrivateKey
					);	
				PrintToDebugWindowOnly16(
					exception->Message->Data()
					);
			}
		});
	}
}