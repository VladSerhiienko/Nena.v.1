#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define _Scenario 3

#if _Scenario == 1

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	try
	{
		SOCKET servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in serv = { 0 };
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = htonl(INADDR_ANY);
		serv.sin_port = htons(18081);
		memset(serv.sin_zero, 0, sizeof(serv.sin_zero));

		if (bind(servSocket, (sockaddr*) &serv, sizeof(serv)) == SOCKET_ERROR)
			throw WSAGetLastError();
		if (listen(servSocket, SOMAXCONN) == SOCKET_ERROR)
			throw WSAGetLastError();
		HANDLE sockEv = CreateEventA(NULL, TRUE, FALSE, NULL);
		if (WSAEventSelect(servSocket, sockEv, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
			throw WSAGetLastError();

		for (;;)
		{
			if (WSAWaitForMultipleEvents(1, &sockEv, FALSE, INFINITE, FALSE) != WSA_WAIT_EVENT_0)
				throw - 1;
			WSANETWORKEVENTS wsaEvents = { 0 };
			if (WSAEnumNetworkEvents(servSocket, sockEv, &wsaEvents) == SOCKET_ERROR)
				throw WSAGetLastError();
			if ((wsaEvents.lNetworkEvents & FD_ACCEPT) == FD_ACCEPT)
			{
				SOCKET clntSocket = accept(servSocket, 0, 0);
				closesocket(clntSocket);
				puts("accept");
			}
			else if (wsaEvents.lNetworkEvents == 0)
			{
				puts("zero event");
			}
		}
	}
	catch (int errorCode)
	{
		printf("Last Error: %d\n", errorCode);
	}
	WSACleanup();
	return 0;
}

#elif _Scenario == 2

#define MAX_CLIENTS 5
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#include <ppl.h>
#include <ppltasks.h>

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{

		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	printf("server addr %s\n", result->ai_addr);

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}


	concurrency::task_group clientRoutines, mainLoop;

	UINT32 taskIndex = 0;
	SOCKET socketToAccept;

	mainLoop.run([&taskIndex, &socketToAccept, ListenSocket, &clientRoutines]()
	{
		printf("Search routine started....\n");

		INT32 i;
		INT32 rc;
		fd_set readableSet;
		fd_set writableSet;
		SOCKET clients[MAX_CLIENTS];
		char buf[1024];
		char buf2[1024];

		timeval t;
		t.tv_sec = 2;
		t.tv_usec = 0;

		for (auto &c : clients) c = INVALID_SOCKET;


		while (
			!concurrency::is_current_task_group_canceling() && 
			!concurrency::is_task_cancellation_requested()
			)
		{
			FD_ZERO(&readableSet);
			FD_ZERO(&writableSet);
			FD_SET(ListenSocket, &readableSet);

			for (auto &c : clients) 
			if (c != INVALID_SOCKET)
			{
				FD_SET(c, &readableSet);
				FD_SET(c, &writableSet);
			}

			rc = select(0, &readableSet, &writableSet, NULL, &t);
			if (rc == SOCKET_ERROR)
			{
				printf("select failed with error: %d\n", WSAGetLastError());
				return;
			}

			if (FD_ISSET(ListenSocket, &readableSet))
			{
				for(auto &c : clients)
				if (c == INVALID_SOCKET)
				{
					c = accept(ListenSocket, NULL, NULL);
					printf("[%3d] new client\n", c);
					break;
				}
			}

			for (auto &c : clients)
			{
				if (c == INVALID_SOCKET) continue;
				if (FD_ISSET(c, &readableSet))
				{
					rc = recv(c, buf, ARRAYSIZE(buf), 0);
					if (rc == 0 || rc == SOCKET_ERROR)
					{
						printf("[%3d] lost client (R)\n", c);
						c = INVALID_SOCKET;
					}
					else
					{
						INT32 idx = rc < ARRAYSIZE(buf) ? rc : ARRAYSIZE(buf) - 1;
						buf[idx] = 0;
						printf("<< [%3d] [%3d] %s\n", c, rc, buf);
					}
				}
				if (FD_ISSET(c, &writableSet))
				{
					INT32 idx = rc < ARRAYSIZE(buf) ? rc : ARRAYSIZE(buf) - 1;
					if (buf[idx - 1] == '?')
						buf[idx - 1] = '!';

					rc = send(c, buf, rc, 0);
					if (rc == 0 || rc == SOCKET_ERROR)
					{
						printf("[%3d] lost client (S)\n", c);
						c = INVALID_SOCKET;
					}
					else
					{
						printf(">> [%3d] [%3d] %s\n", c, rc, buf);
					}
				}

				Sleep(1000);
			}
		}

		for (auto &c : clients)
		if (c != INVALID_SOCKET)
		{
			FD_CLR(c, &readableSet);
			FD_CLR(c, &writableSet);
		}

		FD_CLR(ListenSocket, &readableSet);
		FD_ZERO(&readableSet);
		FD_ZERO(&writableSet);

		for (auto &c : clients)
		if (c != INVALID_SOCKET)
		{
			shutdown(c, SD_BOTH);
			closesocket(c);
		}

		printf("Search routine closed\n");

		/*while (!concurrency::is_current_task_group_canceling())
		{
			printf("Establishing connection...\n");
			socketToAccept = INVALID_SOCKET;
			while (socketToAccept == INVALID_SOCKET)
			{
				socketToAccept = accept(ListenSocket, NULL, NULL);
				if (socketToAccept == INVALID_SOCKET)
				{
					printf("Waiting for clients...\n");
					Sleep(5000);
				}
			}

			printf("Connection established\n");
			taskIndex++;

			clientRoutines.run([taskIndex, socketToAccept, ListenSocket]()
			{
				printf("Starting task %d...\n", taskIndex);

				sockaddr_in socketAddress;
				int length = sizeof socketAddress;

				int result = getsockname(socketToAccept, (SOCKADDR*) &socketAddress, &length);
				if (result) printf("Task %d: getsockname error code = %d\n", taskIndex, WSAGetLastError());

				printf("Task %d: on getsockname = %d\n", taskIndex, result);

				char buffer[64] = "AppTalk!";

				UINT32 bytes;
				while (TRUE)
				{
					if (concurrency::is_task_cancellation_requested()) { printf("Cancelling in task %d...\n", taskIndex); Sleep(2000); break; }
					else if ((result = sendto(socketToAccept, buffer, sizeof buffer, 0, (SOCKADDR*) &socketAddress, length)) > 0)
					{
						printf("Task %d: on sendto = %d\n", taskIndex, result);
						Sleep(2000);
					}
				}
				printf("Quitting task %d...\n", taskIndex);
			});
			printf("Search routine started....\n");
			}*/
	});

	printf("Press and hold escape quit server-like routine\n");

	while (!GetAsyncKeyState(VK_ESCAPE)) Sleep(1000);
	printf("Cancelling...\n");

	mainLoop.cancel();
	printf("Cancelled...\n");

	auto status = mainLoop.wait();
	printf("Press any key to exit (status = %d)\n", status);

	getchar();
	return EXIT_SUCCESS;
}

#elif _Scenario == 3

#include "AppTalk.h"

BOOL OnNewClient(Nena::AppTalk::Callstation *s, Nena::AppTalk::Sock sock)
{
	printf("[%d] new client\n", sock);
	return TRUE; // accept all clients
}

void OnClientLost(Nena::AppTalk::Callstation *s, Nena::AppTalk::Sock sock)
{
	printf("[%d] client lost\n", sock);
}

void OnDataReceived(Nena::AppTalk::Callstation *s, Nena::AppTalk::Sock sock, Nena::AppTalk::Byte *data, INT32 sz)
{
	data[sz] = 0;
	printf("[%d] recv %3d bytes %s\n", sock, sz, data);
}

BOOL OnSendingData(Nena::AppTalk::Callstation *s, Nena::AppTalk::Sock sock, Nena::AppTalk::Byte *data, INT32 max, INT32 &out)
{
	int num = rand();
	char thisbuffer[32];
	_itoa_s(num, thisbuffer, 10);

	memcpy(data, thisbuffer, sizeof thisbuffer);
	//memcpy_s(data, sizeof thisbuffer, thisbuffer, max);
	out = ::strlen(thisbuffer);

	printf("[%d] send %3d bytes %s\n", sock, out, data);
	return TRUE;
}

int __cdecl main(int argc, char **argv)
{
	BOOL bstatus = FALSE;

	Nena::AppTalk talk;
	bstatus = talk.Init();

	Nena::AppTalk::Callstation station;
	station.SetOnNewCallback(&OnNewClient);
	station.SetOnLostCallback(&OnClientLost);
	station.SetOnDataSendCallback(&OnSendingData);
	station.SetOnDataReceivedCallback(&OnDataReceived);

	station.Init();
	station.RunSimpleStationProc(8);

	printf("-------------------------------------------\n");
	printf("Press and hold escape for 2 seconds to quit\n");
	printf("-------------------------------------------\n");

	while (!GetAsyncKeyState(VK_ESCAPE)) Sleep(2000);

	printf("-------------------------------------------\n");
	printf("Cancelling...\n");

	station.CancelSimpleStationProc(FALSE);
	auto tstatus = station.WaitForSimpleStationProc();

	printf("-------------------------------------------\n");
	printf("Cancelled...\n");

	printf("-------------------------------------------\n");
	printf("Press any key to exit (status = %d)\n", tstatus);
	printf("-------------------------------------------\n");

	talk.Release();

	getchar();
	return EXIT_SUCCESS;
}

#endif


