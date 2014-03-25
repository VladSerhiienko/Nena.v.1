#include "RemoteControl.h"
#include <Nena\VideoTrackingGesturesPerc.h>

Demo::RemoteControl g_controller;
static CHAR s_msg[512] = { 0 };

struct GestureCallback :
	protected PXCGesture::Gesture::Handler,
	protected PXCGesture::Alert::Handler
{
	typedef PXCGesture::Gesture::Handler *HGesture;
	typedef PXCGesture::Alert::Handler *HAlert;

	inline HAlert GetAlertHandler() { return this; }
	inline HGesture GetGestureHandler() { return this; }

	virtual void PXCAPI OnAlert(_In_ PXCGesture::Alert *alert) override
	{
	}

	virtual void PXCAPI OnGesture(_In_ PXCGesture::Gesture *data) override
	{
		using namespace Nena::Video;
		OutputDebugStringA("\t-> ");

		if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT))
			g_controller.SendMessageToTargets(WM_KEYDOWN, VK_LEFT, 0),
			sprintf_s(s_msg, "\tSWIPE_LEFT(%d)\n", data->label),
			std::cout << (s_msg);
		else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT))
			g_controller.SendMessageToTargets(WM_KEYDOWN, VK_RIGHT, 0),
			sprintf_s(s_msg, "\tSWIPE_RIGHT(%d)\n", data->label),
			std::cout << (s_msg);
		else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN))
			g_controller.SendMessageToTargets(WM_KEYDOWN, VK_DOWN, 0),
			sprintf_s(s_msg, "\tSWIPE_DOWN(%d)\n", data->label),
			std::cout << (s_msg);
		else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_UP))
			g_controller.SendMessageToTargets(WM_KEYDOWN, VK_UP, 0),
			sprintf_s(s_msg, "\tSWIPE_UP(%d)\n", data->label),
			std::cout << (s_msg);
	}
};


void EnableCrt()
{
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
}

void PrintUsage()
{
	std::cout << "usage: \n";
	std::cout << "\t -print\t prints all top level windows info to <-f>\n";
	std::cout << "\t -emit \t read focus data from <-f> and emits key events\n";
	std::cout << "\t -file \t target file name\n";
	std::cout << "\nexample1: -print -file \"C:\\toplevelwindows.txt\"\n";
	std::cout << "example2: -emit -file \"C:\\focuswindows.txt\"\n";
	std::cout << "press any key to exit...\n";
	std::cin.get();
}

int main(int argc, char *argv [])
{
	using namespace Nena::Video;
	using namespace Nena::Video::Perc;

	::std::string arg, file = "focuswindows.txt";
	EnableCrt();

	argc = 4;

	char *argv2[] = 
	{
		"module-path",
		"-emit",
		//"-print",
		"-file",
		"focuswindows.txt",
		//"toplevelwindows.txt",
	};

	argv = argv2;

	if (argc < 4)
	{
		PrintUsage();
		return EXIT_FAILURE;
	}
	else
	{
		int argNum = 1;
		for (; argNum < argc; argNum++)
		{
			arg = argv[argNum];

			if (arg == "-print")
			{
				if (argNum + 2 < argc)
				{
					file = argv[argNum + 2];
					if (FAILED(g_controller.WriteToplevelWindows(file))) return EXIT_FAILURE;
					else return EXIT_SUCCESS;
				}
				else
				{
					PrintUsage();
					return EXIT_FAILURE;
				}
			}
			else if (arg == "-emit")
			{
				if (argNum + 2 < argc)
				{
					file = argv[argNum + 2];
				}

				if (FAILED(g_controller.ReadFocusData(file)))
					return EXIT_FAILURE;

				g_controller.UpdateFocusHandles();
				break;
			}
		}
	}

	pxcStatus sts;
	Perc::Capture capture;

	GestureCallback eventsOutput;

	Tracking::Gesture gesture(&capture);		if (!gesture.IsValid()) return EXIT_FAILURE;

	gesture.SearchProfile();					if (!gesture.IsValid()) return EXIT_FAILURE;
	gesture.SubmitRequirements(&capture);		if (!gesture.IsValid()) return EXIT_FAILURE;
	capture.LocateStreams();					if (!capture.IsValid()) return EXIT_FAILURE;
	gesture.SetProfile();						if (!gesture.IsValid()) return EXIT_FAILURE;

	gesture.Subscribe(eventsOutput.GetGestureHandler());	if (!gesture.IsValid()) return EXIT_FAILURE;

	auto sample = capture.GetSampleFromDesc(gesture.DescIndex);

	PXCSmartSPArray sps(2);
	PXCSmartArray<PXCImage> images;

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		sts = sample->SubmitReadTask(images.ReleaseRefs(), sps.ReleaseRef(0));		if (Utility::Failed(sts)) break;
		sts = gesture.SubmitProcessTask(images, sps.ReleaseRef(1));					if (Utility::Failed(sts)) break;
		sps.SynchronizeEx();														if (Utility::Failed(sts)) break;
	}

	return EXIT_SUCCESS;

	return 0;
}