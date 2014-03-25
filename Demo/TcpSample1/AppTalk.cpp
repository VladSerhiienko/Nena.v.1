#include "AppTalk.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment (lib, "Mswsock.lib")

#define _Nena_AppTalk_IsNotTaskCancellationRequested \
	!concurrency::is_current_task_group_canceling() && \
	!concurrency::is_task_cancellation_requested()

#define _Nena_AppTalk_IsTaskCancellationRequested \
	concurrency::is_current_task_group_canceling() || \
	concurrency::is_task_cancellation_requested()

namespace Nena
{

	BOOL AppTalk::Init()
	{
		errno_t error = WSAStartup(MAKEWORD(2, 2), &Data); if (error)
		{
			printf("[ERROR] AppTalk::Init() %d\n", error);
			return FALSE;
		}
		return TRUE;
	}

	void AppTalk::Release()
	{
		errno_t error = WSACleanup(); if (error)
		{
			printf("[ERROR] AppTalk::Release() %d\n", error);
		}
	}

	void AppTalk::ReleaseSock(Sock &s, INT32 how, errno_t *onsd, errno_t *oncs)
	{
		if (IsInvalidSock(s)) return;
		errno_t sd, cs;
		sd = ::shutdown(s, how);
		cs = ::closesocket(s);
		s = InvalidSocket;
		if (onsd) (*onsd) = sd;
		if (oncs) (*oncs) = cs;
	}

	AppTalk::Callbox::Callbox()
	{
	}

	AppTalk::Callbox::~Callbox()
	{
	}

	BOOL AppTalk::Callbox::Init()
	{

		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = AF_UNSPEC;
		m_hints.ai_socktype = SOCK_STREAM;
		m_hints.ai_protocol = IPPROTO_TCP;

		char port[32] = { 0 };
		::_itoa_s(DefaultPort, port, 10);

		// Resolve the server address and port
		errno_t error = ::getaddrinfo(NULL, port, &m_hints, &m_result);
		if (error)
		{
			printf("[ERROR] AppTalk::Callbox::Init() in getaddrinfo() %d\n", error);
			return FALSE;
		}

		for (AddressInfo *ptr = m_result; ptr != NULL; ptr = ptr->ai_next)
		{
			m_client = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (IsInvalidSock(m_client))
			{
				printf("[ERROR] AppTalk::Callbox::Init() in socket() %d\n", WSAGetLastError());
				::freeaddrinfo(m_result);
				return FALSE;
			}

			error = ::connect(m_client, ptr->ai_addr, (INT32) ptr->ai_addrlen);
			if (error == SocketError)
			{
				printf("[ERROR] AppTalk::Callbox::Init() in connect() %d\n", WSAGetLastError());
				ReleaseSock(m_client);
				continue;
			}

			break;
		}

		::freeaddrinfo(m_result);
		if (IsInvalidSock(m_client))
		{
			ReleaseSock(m_client);
			return FALSE;
		}

		return TRUE;
	}

	void AppTalk::Callbox::Release()
	{
		CancelSimpleBoxProc(TRUE);
		ReleaseSock(m_client);
	}

	void AppTalk::Callbox::RunSimpleBoxProc(BOOL dlg)
	{
		dlg = !!dlg;
		m_flow.run([this, dlg]()
		{
			INT32 nbytes = 0;
			BOOL nondlg = !dlg;
			errno_t error = NO_ERROR;
			StaticBuffer bytes = { 0 };
			SockSet readableSet = { 0 };
			SockSet writableSet = { 0 };

			timeval t;
			t.tv_sec = DefaultSelectTimeoutSecs;
			t.tv_usec = DefaultSelectTimeoutUsecs;

			while (_Nena_AppTalk_IsNotTaskCancellationRequested)
			{
				FD_ZERO(&readableSet);
				FD_ZERO(&writableSet);
				FD_SET(m_client, &readableSet);
				FD_SET(m_client, &writableSet);

				error = ::select(0, &readableSet, &writableSet, NULL, &t);
				if (error == SocketError)
				{
					printf("[ERR] AppTalk::Callbox::Run() in flow.run() in select() %d\n", WSAGetLastError());
					return;
				}

				if (FD_ISSET(m_client, &readableSet))
				{
					nbytes = recv(m_client, bytes, BufferSize(bytes), 0);
					if (nbytes == 0 || nbytes == SocketError) { m_onPostLost(this, m_client); break; }
					m_onPostReceived(this, m_client, bytes, nbytes);

					if (dlg && FD_ISSET(m_client, &writableSet))
					{
						m_onPreSend(this, m_client, bytes, BufferSize(bytes), nbytes);
						nbytes = send(m_client, bytes, nbytes, 0);
						if (nbytes == 0 || nbytes == SocketError) { m_onPostLost(this, m_client); break; }
					}
				}

				if (nondlg && FD_ISSET(m_client, &writableSet))
				{
					m_onPreSend(this, m_client, bytes, BufferSize(bytes), nbytes);
					nbytes = send(m_client, bytes, nbytes, 0);
					if (nbytes == 0 || nbytes == SocketError) { m_onPostLost(this, m_client); break; }
				}
			}

			FD_CLR(m_client, &readableSet);
			FD_CLR(m_client, &writableSet);
			FD_ZERO(&readableSet);
			FD_ZERO(&writableSet);

			ReleaseSock(m_client);
		});
	}

	void AppTalk::Callbox::CancelSimpleBoxProc(BOOL wait)
	{
		m_flow.cancel();
	}

	BOOL AppTalk::Callbox::IsSimpleBoxProcCanceling()
	{
		return m_flow.is_canceling();
	}

	AppTalk::TaskStatus AppTalk::Callbox::WaitForSimpleBoxProc()
	{
		return m_flow.wait();
	}

	AppTalk::Callstation::Callstation()
	{
		m_result = NULL;
		m_onPreNew = NULL;
		m_onPreSend = NULL;
		m_onPostLost = NULL;
		m_onPostReceived = NULL;
		m_server = InvalidSocket;
		ZeroMemory(&m_hints, sizeof m_hints);
	}

	AppTalk::Callstation::~Callstation()
	{
		Release();
	}

	BOOL AppTalk::Callstation::Init()
	{

		ZeroMemory(&m_hints, sizeof m_hints);
		m_hints.ai_family = AF_INET;
		m_hints.ai_socktype = SOCK_STREAM;
		m_hints.ai_protocol = IPPROTO_TCP;
		m_hints.ai_flags = AI_PASSIVE;

		char port[32] = { 0 };
		::_itoa_s(DefaultPort, port, 10);
		
		errno_t error = ::getaddrinfo(NULL, port, &m_hints, &m_result); 
		if (error)
		{
			printf("[ERROR] AppTalk::Callstation::Init() in getaddrinfo() %d\n", error);
			return FALSE;
		}

		m_server = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol); 
		if (IsInvalidSock(m_server))
		{
			printf("[ERROR] AppTalk::Callstation::Init() in socket() %d\n", WSAGetLastError());
			::freeaddrinfo(m_result);
			return FALSE;
		}

		error = ::bind(m_server, m_result->ai_addr, (INT32) m_result->ai_addrlen);
		if (error == SocketError)
		{
			printf("[ERROR] AppTalk::Callstation::Init() in bind() %d\n", WSAGetLastError());
			::freeaddrinfo(m_result);
			::closesocket(m_server);
			return FALSE;
		}

		::freeaddrinfo(m_result);

		error = ::listen(m_server, MaxListenQueueSize);
		if (error == SocketError)
		{
			printf("[ERROR] AppTalk::Callstation::Init() in listen() %d\n", WSAGetLastError());
			::closesocket(m_server);
			return FALSE;
		}
		
		return TRUE;
	}

	void AppTalk::Callstation::Release()
	{
		CancelSimpleStationProc(TRUE);
		ReleaseSock(m_server);
	}

	void AppTalk::Callstation::RunSimpleStationProc(UINT32 nclients, BOOL dlg)
	{
		dlg = !!dlg;
		m_flow.run([this, nclients, dlg]()
		{
			errno_t error = NO_ERROR;
			INT32 bytes = 0;
			UINT32 clientIndex = -1;
			Sock clients[MaxClients];
			SockSet readableSet = { 0, InvalidSocket };
			SockSet writableSet = { 0, InvalidSocket };
			BOOL nondlg = !dlg;

			timeval t;
			t.tv_sec = DefaultSelectTimeoutSecs;
			t.tv_usec = DefaultSelectTimeoutUsecs;

			StaticBuffer buffer;

			for (auto &c : clients) c = INVALID_SOCKET;

			while (_Nena_AppTalk_IsNotTaskCancellationRequested)
			{
				FD_ZERO(&readableSet);
				FD_ZERO(&writableSet);
				FD_SET(m_server, &readableSet);

				clientIndex = -1;
				for (auto &c : clients)
				if (clientIndex++, IsValidSock(c) && (clientIndex < nclients))
				{
					FD_SET(c, &readableSet);
					FD_SET(c, &writableSet);
				} else break;

				error = ::select(0, &readableSet, &writableSet, NULL, &t);
				if (error == SocketError)
				{
					printf("[ERR] AppTalk::Callstation::Run() in flow.run() in select() %d\n", WSAGetLastError());
					return;
				}

				if (FD_ISSET(m_server, &readableSet))
				{
					clientIndex = -1;
					for (auto &c : clients)
					if (clientIndex++, IsInvalidSock(c) && (clientIndex < nclients))
					{
						c = ::accept(m_server, NULL, NULL);
						if (!m_onPreNew(this, c)) ReleaseSock(c);
						break;
					}
				}

				clientIndex = -1;
				for (auto &c : clients)
				{
					if (clientIndex++, IsInvalidSock(c) || (clientIndex >= nclients)) continue;

					BOOL brecv = FD_ISSET(c, &readableSet);
					BOOL bsend = FD_ISSET(c, &writableSet);

					if (brecv)
					{
						bytes = recv(c, buffer, BufferSize(buffer), 0);
						if (bytes == 0 || bytes == SocketError)
						{
							m_onPostLost(this, c);
							ReleaseSock(c);
						}
						else
						{
							m_onPostReceived(this, c, buffer, bytes);

							if (bsend && dlg)
							{
								INT32 out;
								if (m_onPreSend(this, c, buffer, BufferSize(buffer), out))
								{
									bytes = send(c, buffer, out, 0);
									if (bytes == 0 || bytes == SocketError)
									{
										m_onPostLost(this, c);
										ReleaseSock(c);
									}
								}
							}
						}

						if (bsend && nondlg)
						{
							INT32 out;
							if (m_onPreSend(this, c, buffer, BufferSize(buffer), out))
							{
								bytes = send(c, buffer, out, 0);
								if (bytes == 0 || bytes == SocketError)
								{
									m_onPostLost(this, c);
									ReleaseSock(c);
								}
							}
						}
					}

					//Sleep(1000);
				}
			}

			FD_CLR(m_server, &readableSet);

			clientIndex = -1;
			for (auto &c : clients) 
			if (clientIndex++, IsValidSock(c) && (clientIndex < nclients))
			{
				FD_CLR(c, &readableSet);
				FD_CLR(c, &writableSet);
			}

			FD_ZERO(&readableSet);
			FD_ZERO(&writableSet);

			clientIndex = -1;
			for (auto &c : clients)
			if (clientIndex++, IsValidSock(c) && (clientIndex < nclients))
				ReleaseSock(c);
		});
	}

	void AppTalk::Callstation::CancelSimpleStationProc(BOOL wait)
	{
		m_flow.cancel();
		if (wait) m_flow.wait();
	}


	BOOL AppTalk::Callstation::IsSimpleStationProcCanceling()
	{
		return m_flow.is_canceling();
	}

	AppTalk::TaskStatus AppTalk::Callstation::WaitForSimpleStationProc()
	{
		return m_flow.wait();
	}

}