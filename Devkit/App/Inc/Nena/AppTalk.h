#include "App.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#include <ppl.h>
#include <ppltasks.h>

#ifndef __NENA_APPTALK_INCLUDED__
#define __NENA_APPTALK_INCLUDED__

namespace Nena
{
	struct App::Talk
	{
		struct Callbox;
		struct Callstation;

		static UINT32 const MaxClients = FD_SETSIZE - 2;
		static UINT32 const DefaultPort = 27015;
		static INT32 const SocketError = SOCKET_ERROR;
		static UINT32 const InvalidSocket = (UINT32) INVALID_SOCKET;
		static UINT32 const MaxBufferByteSize = 1024;
		static UINT32 const ReservedBufferByteSize = 512;
		static UINT32 const MaxListenQueueSize = SOMAXCONN;
		static UINT32 const DefaultSelectTimeoutSecs = 2;
		static UINT32 const DefaultSelectTimeoutUsecs = 0;

		typedef CHAR Byte;
		typedef ::SOCKET Sock;
		typedef ::fd_set SockSet;
		typedef ::addrinfo AddressInfo;
		typedef concurrency::task_group TaskGroup;
		typedef concurrency::task_group_status TaskStatus;

		static BOOL inline IsValidSock(Sock s) { return s != InvalidSocket; }
		static BOOL inline IsInvalidSock(Sock s) { return s == InvalidSocket; }
		static void ReleaseSock(Sock &s, INT32 how = SD_BOTH, errno_t *onsd = NULL, errno_t *oncs = NULL);
		template <INT32 _Sz> static inline INT32 BufferSize(Byte(&data)[_Sz]) { return _Sz; }
		template <UINT32 _Sz> static inline UINT32 BufferSizeU(Byte(&data)[_Sz]) { return _Sz; }
		template <typename _TySz, _TySz _Sz> static inline _TySz BufferSizeT(Byte(&data)[_Sz]) { return _Sz; }

		struct Callbox
		{
			typedef Byte StaticBuffer[MaxBufferByteSize + ReservedBufferByteSize];

			typedef BOOL(*NewClientCallbackTy)(Callbox *box, Sock sock);
			typedef void(*LostClientCallbackTy)(Callbox *box, Sock sock);
			typedef void(*DataReceivedCallbackTy)(Callbox *box, Sock sock, Byte *data, INT32 recv);
			typedef BOOL(*DataSendCallbackTy)(Callbox *box, Sock sock, Byte *data, INT32 max, INT32 &out);

			Callbox();
			~Callbox();

			BOOL Init();
			void Release();

			void RunSimpleBoxProc(BOOL dialogMode = TRUE);
			void CancelSimpleBoxProc(BOOL wait = TRUE);
			BOOL IsSimpleBoxProcCanceling();
			TaskStatus WaitForSimpleBoxProc();

			inline void SetOnLostCallback(LostClientCallbackTy c) { if (c) m_onPostLost = c; }
			inline void SetOnDataSendCallback(DataSendCallbackTy c) { if (c) m_onPreSend = c; }
			inline void SetOnDataReceivedCallback(DataReceivedCallbackTy c) { if (c) m_onPostReceived = c; }

		protected:

			TaskGroup m_flow;
			Sock m_client;
			AddressInfo *m_result;
			AddressInfo m_hints;

			DataSendCallbackTy m_onPreSend;
			LostClientCallbackTy m_onPostLost;
			DataReceivedCallbackTy m_onPostReceived;
		};

		struct Callstation
		{
			typedef Byte StaticBuffer[MaxBufferByteSize + ReservedBufferByteSize];

			typedef BOOL(*NewClientCallbackTy)(Callstation *station, Sock sock);
			typedef void(*LostClientCallbackTy)(Callstation *station, Sock sock);
			typedef void(*DataReceivedCallbackTy)(Callstation *station, Sock sock, Byte *data, INT32 recv);
			typedef BOOL(*DataSendCallbackTy)(Callstation *station, Sock sock, Byte *data, INT32 max, INT32 &out);

			Callstation();
			~Callstation();

			BOOL Init();
			void Release();

			void RunSimpleStationProc(UINT32 clients = MaxClients, BOOL dialogMode = TRUE);
			void CancelSimpleStationProc(BOOL wait = TRUE);
			BOOL IsSimpleStationProcCanceling();
			TaskStatus WaitForSimpleStationProc();

			inline void SetOnNewCallback(NewClientCallbackTy c) { if (c) m_onPreNew = c; }
			inline void SetOnLostCallback(LostClientCallbackTy c) { if (c) m_onPostLost = c; }
			inline void SetOnDataSendCallback(DataSendCallbackTy c) { if (c) m_onPreSend = c; }
			inline void SetOnDataReceivedCallback(DataReceivedCallbackTy c) { if (c) m_onPostReceived = c; }

		protected:

			TaskGroup m_flow;

			Sock m_server;
			AddressInfo *m_result;
			AddressInfo m_hints;

			DataSendCallbackTy m_onPreSend;
			NewClientCallbackTy m_onPreNew;
			LostClientCallbackTy m_onPostLost;
			DataReceivedCallbackTy m_onPostReceived;

		};

		BOOL Init();
		void Release();

	protected:

		WSADATA Data;

	};
}

#endif // !__NENA_APPTALK_INCLUDED__
