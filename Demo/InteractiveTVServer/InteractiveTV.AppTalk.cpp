#include "InteractiveTV.h"
namespace Presentation
{
	char InteractiveTV::s_msg[512] = { 0 };

	std::string InteractiveTV::AppTalk::ToString(MessageType t)
	{
		switch (t)
		{
		case MessageType::Request: return std::string(__ITV_AppTalk_CeRequest);
		case MessageType::EventSubscribe: return std::string(__ITV_AppTalk_CeEventSubscribe);
		case MessageType::EventUnsubscribe: return std::string(__ITV_AppTalk_CeEventUnsubscribe);
		case MessageType::Unknown: return std::string(__ITV_AppTalk_CeUnknown);
		default: return std::string(__ITV_AppTalk_CeUnknown);
		}
	}

	std::string InteractiveTV::AppTalk::ToString(MessageArgs t)
	{
		switch (t)
		{
		case MessageArgs::Name: return std::string(__ITV_AppTalk_CeRequestName);
		case MessageArgs::Next: return std::string(__ITV_AppTalk_CeRequestName);
		case MessageArgs::GestureNavigation: return std::string(__ITV_AppTalk_CeEventGestureNavigation);
		case MessageArgs::GestureSwipeRight: return std::string(__ITV_AppTalk_CeEventGestureSwipeRight);
		case MessageArgs::GestureSwipeLeft: return std::string(__ITV_AppTalk_CeEventGestureSwipeLeft);
		case MessageArgs::GestureSwipeDown: return std::string(__ITV_AppTalk_CeEventGestureSwipeDown);
		case MessageArgs::GestureSwipeUp: return std::string(__ITV_AppTalk_CeEventGestureSwipeUp);
		case MessageArgs::FaceDetected: return std::string(__ITV_AppTalk_CeEventFaceDetected);
		case MessageArgs::FaceRecognized: return std::string(__ITV_AppTalk_CeEventFaceRecognized);
		case MessageArgs::Unknown: return std::string(__ITV_AppTalk_CeUnknown);
		default: return std::string(__ITV_AppTalk_CeUnknown);
		}
	}

	InteractiveTV::AppTalk::MessageType InteractiveTV::AppTalk::ToMessageType(std::string const &s)
	{
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeRequest)) return MessageType::Request;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventSubscribe)) return MessageType::EventSubscribe;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventUnsubscribe)) return MessageType::EventUnsubscribe;
		return MessageType::Unknown;
	}

	InteractiveTV::AppTalk::MessageArgs InteractiveTV::AppTalk::ToMessageArgs(std::string const &s)
	{
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventFaceDetected)) return MessageArgs::FaceDetected;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventFaceRecognized)) return MessageArgs::FaceRecognized;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventGestureNavigation)) return MessageArgs::GestureNavigation;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventGestureSwipeDown)) return MessageArgs::GestureSwipeDown;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventGestureSwipeLeft)) return MessageArgs::GestureSwipeLeft;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventGestureSwipeRight)) return MessageArgs::GestureSwipeRight;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeEventGestureSwipeUp)) return MessageArgs::GestureSwipeUp;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeRequestName)) return MessageArgs::Name;
		if (!::strcmp(s.c_str(), __ITV_AppTalk_CeRequestNext)) return MessageArgs::Next;
		return MessageArgs::Unknown;
	}

	BOOL InteractiveTV::AppTalk::TryParseText(std::string const &s, Entry::Vector &tokens)
	{
		using namespace std;

		Entry entry;
		std::vector<std::string> stringEntries = Split(s, ';');
		for (auto &stringEntry : stringEntries)
		{
			vector<string> stringTokens
			{
			istream_iterator<string> { std::istringstream(stringEntry) },
			istream_iterator<string> { }
		};

			if (stringTokens.size() != 2) continue;
			entry.type = ToMessageType(stringTokens.front());
			entry.args = ToMessageArgs(stringTokens.back());
			tokens.push_back(entry);
		}

		return true;
	}

	std::vector<std::string> &InteractiveTV::AppTalk::Split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s); std::string item;
		while (std::getline(ss, item, delim)) elems.push_back(item);
		return elems;
	}


	std::vector<std::string> InteractiveTV::AppTalk::Split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		Split(s, delim, elems);
		return elems;
	}

	BOOL InteractiveTV::AppTalk::OnNewClient(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock)
	{
		printf("[%d] new client\n", sock);

		InteractiveTV::Get()->Talk.Reg[sock].Id = sock;
		InteractiveTV::Get()->Talk.Reg[sock].Send.push("resquest: name;");

		return TRUE; // accept all clients
	}

	void InteractiveTV::AppTalk::OnClientLost(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock)
	{
		printf("[%d] client lost\n", sock);
		InteractiveTV::Get()->Talk.Reg.unsafe_erase(sock);
	}

	void InteractiveTV::AppTalk::OnDataReceived(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock, Nena::App::Talk::Byte *data, INT32 sz)
	{
		data[sz] = 0;
		//printf("[%d] recv %3d bytes %s\n", sock, sz, data);
	}

	BOOL InteractiveTV::AppTalk::OnSendingData(Nena::App::Talk::Callstation *s, Nena::App::Talk::Sock sock, Nena::App::Talk::Byte *data, INT32 maxsz, INT32 &out)
	{
		ZeroMemory(data, maxsz);

		std::string latest;
		if (!InteractiveTV::Get()->Talk.Msgs.try_pop(latest))
			//return FALSE;
			latest = "event-none;";// ,
			//Sleep(200);

		out = latest.size();
		memcpy(data, latest.c_str(), out);

		//printf("[%d] send %3d bytes %s\n", sock, out, data);
		return TRUE;
	}

	InteractiveTV::AppTalk::AppTalk()
	{
		Setup();
	}

	InteractiveTV::AppTalk::~AppTalk()
	{
		Close();
	}

	void InteractiveTV::AppTalk::Close()
	{
		Station.CancelSimpleStationProc(FALSE);
		Station.WaitForSimpleStationProc();
	}

	void InteractiveTV::AppTalk::Setup()
	{
		Station.SetOnNewCallback(&InteractiveTV::AppTalk::OnNewClient);
		Station.SetOnLostCallback(&InteractiveTV::AppTalk::OnClientLost);
		Station.SetOnDataSendCallback(&InteractiveTV::AppTalk::OnSendingData);
		Station.SetOnDataReceivedCallback(&InteractiveTV::AppTalk::OnDataReceived);

	}

	void InteractiveTV::AppTalk::Launch()
	{
		Station.Init();
		Station.RunSimpleStationProc(16, FALSE);
	}

	InteractiveTV::InteractiveTV() : VideoCapturer(), GestureTracker(&VideoCapturer)
	{
		Supp.Init();
	}

	InteractiveTV::~InteractiveTV()
	{
		Supp.Release();
	}

	void InteractiveTV::Init()
	{
		Talk.Setup();

		if (!VideoCapturer.IsValid() ||
			!GestureTracker.IsValid())
			return;


		GestureTracker.SearchProfile();
		_ASSERT_EXPR(GestureTracker.IsValid(), L"GestureTracker.SearchProfile()");

		GestureTracker.SubmitRequirements(&VideoCapturer);
		_ASSERT_EXPR(GestureTracker.IsValid(), L"GestureTracker.SubmitRequirements()");

		VideoCapturer.LocateStreams();
		_ASSERT_EXPR(VideoCapturer.IsValid(), L"VideoCapturer.LocateStreams()");

		GestureTracker.SetProfile();
		_ASSERT_EXPR(GestureTracker.IsValid(), L"GestureTracker.SetProfile()");

		GestureTracker.Subscribe(GestureTrackerOutput.GetGestureHandler());
		_ASSERT_EXPR(GestureTracker.IsValid(), L"GestureTracker.Subscribe()");

		VideoSampler = VideoCapturer.GetSampleFromDesc(
			GestureTracker.DescIndex
			);
		_ASSERT_EXPR(!!VideoSampler, L"!!VideoSampler");

	}

	void InteractiveTV::Launch()
	{
		Talk.Launch();
		LaunchGestureTracking();
	}

	void InteractiveTV::Close()
	{
		CancelGestureTracking();
		Talk.Close();
	}

	InteractiveTV *InteractiveTV::Get()
	{
		static InteractiveTV itv;
		return &itv;
	}

	BOOL InteractiveTV::IsValid()
	{
		return
			VideoCapturer.IsValid() &&
			GestureTracker.IsValid() &&
			VideoSampler->IsValid();

	}

}