#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ppl.h>
#include <ppltasks.h>
#include <concurrent_queue.h>
#include <concurrent_unordered_map.h>

#include <Nena\AppTalk.h>
#include <Nena\VideoTrackingGesturesPerc.h>

#define __ITV_AppTalk_CeUnknown ""
#define __ITV_AppTalk_CeRequest "request:"
#define __ITV_AppTalk_CeRequestName "name"
#define __ITV_AppTalk_CeRequestNext "next"
#define __ITV_AppTalk_CeEventSubscribe "event-subscribe:"
#define __ITV_AppTalk_CeEventUnsubscribe "event-unsubscribe:"
#define __ITV_AppTalk_CeEventGestureNavigation "gesture-navigation"
#define __ITV_AppTalk_CeEventGestureSwipeRight "gesture-swipe-right"
#define __ITV_AppTalk_CeEventGestureSwipeLeft "gesture-swipe-left"
#define __ITV_AppTalk_CeEventGestureSwipeDown "gesture-swipe-down"
#define __ITV_AppTalk_CeEventGestureSwipeUp "gesture-swipe-up"
#define __ITV_AppTalk_CeEventFaceDetected "face-detected"
#define __ITV_AppTalk_CeEventFaceRecognized "face-recognized"

namespace Presentation
{
	struct InteractiveTV
	{
		struct AppTalk
		{

			enum class MessageType
			{
				Unknown,

				Request = 1 << 0,
				EventSubscribe = 1 << 1,
				EventUnsubscribe = 1 << 2,
			};

			enum class MessageArgs
			{
				Unknown,

				Name = 1 << 0,
				Next = 1 << 1,
				GestureSwipeRight = 1 << 2,
				GestureSwipeLeft = 1 << 3,
				GestureSwipeDown = 1 << 4,
				GestureSwipeUp = 1 << 5,
				GestureNavigation = 
					GestureSwipeRight | 
					GestureSwipeLeft | 
					GestureSwipeDown | 
					GestureSwipeUp,
				FaceDetected = 1 << 6,
				FaceRecognized = 1 << 7,

			};

			struct Entry
			{
				typedef std::vector<Entry> Vector;
				MessageType type = MessageType::Unknown;
				MessageArgs args = MessageArgs::Unknown;
			};

			struct Client
			{
				std::string Name;
				Nena::App::Talk::Sock Id;
				AppTalk::MessageArgs Subscription;
				concurrency::concurrent_queue<std::string> Received; // process
				concurrency::concurrent_queue<std::string> Send; // processed

				void Subscribe(AppTalk::MessageArgs);
				void Unsubscribe(AppTalk::MessageArgs);
				void IsSubscribed(AppTalk::MessageArgs);
				void ProcessMessage(std::string);
			};

			typedef concurrency::concurrent_unordered_map<Nena::App::Talk::Sock, Client> Registry;

			static std::string ToString(MessageType);
			static std::string ToString(MessageArgs);
			static MessageType ToMessageType(std::string const&);
			static MessageArgs ToMessageArgs(std::string const&);

			static BOOL TryParseText(std::string const&, Entry::Vector&);
			static std::vector<std::string> &Split(const std::string &, char, std::vector<std::string> &);
			static std::vector<std::string> Split(const std::string &, char);

			static BOOL OnNewClient(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock);
			static void OnClientLost(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock);
			static void OnDataReceived(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock, Nena::App::Talk::Byte *data, INT32 sz);
			static BOOL OnSendingData(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock, Nena::App::Talk::Byte *data, INT32 max, INT32 &out);

			Nena::App::Talk::Callstation Station;
			concurrency::concurrent_queue<std::string> Msgs;
			Registry Reg;

			AppTalk();
			~AppTalk();

			void Setup();
			void Launch();
			void Close();

		};

		struct GestureCallback :
			protected PXCGesture::Gesture::Handler,
			protected PXCGesture::Alert::Handler
		{
			typedef PXCGesture::Alert::Handler *HAlert;
			typedef PXCGesture::Gesture::Handler *HGesture;
			inline HAlert GetAlertHandler() { return this; }
			inline HGesture GetGestureHandler() { return this; }
			virtual void PXCAPI OnAlert(_In_ PXCGesture::Alert *alert) override;
			virtual void PXCAPI OnGesture(_In_ PXCGesture::Gesture *data) override;
		};

		AppTalk Talk;
		Nena::App::Talk Supp;

		GestureCallback GestureTrackerOutput;
		Nena::Video::Perc::Sample *VideoSampler;
		Nena::Video::Perc::Capture VideoCapturer;
		Nena::Video::Perc::Tracking::Gesture GestureTracker;

		InteractiveTV();
		~InteractiveTV();

		void Init();
		void Launch();
		void Close();
		BOOL IsValid();
		void ProcessGesture(PXCGesture::Alert *);
		void ProcessGesture(PXCGesture::Gesture *);

		static CHAR s_msg[512];
		static InteractiveTV *Get();

	private:

		concurrency::task_group m_flow;
		concurrency::task_group_status m_trackingStatus;

		void GestureTrackingRoutine();
		void LaunchGestureTracking();
		void CancelGestureTracking();

	};
}