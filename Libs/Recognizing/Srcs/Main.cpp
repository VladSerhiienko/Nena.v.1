
#include "Video.h"
#include <Psapi.h>


#if 0

std::map<HWND, std::wstring> g_windows;
std::map<HWND, std::wstring> g_images;
std::map<HWND, std::wstring> g_images2;
std::map<HWND, std::wstring> g_images3;
std::map<HWND, std::wstring> g_images4;
std::map<HWND, std::wstring> g_classes;
std::vector<HWND> g_matched;

BOOL CALLBACK enumWindowsProc(HWND hWnd,LPARAM lParam)
{
	/*if (!::IsIconic(hWnd))
	{
		return TRUE;
	}*/

	WCHAR buffer[1024] = { L"\0" };
	INT32 len = GetWindowTextW(hWnd, buffer, ARRAYSIZE(buffer));
	if (!len) return TRUE;
	g_windows[hWnd] = buffer;

	GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer));
	g_classes[hWnd] = buffer;

	DWORD processId;
	GetWindowThreadProcessId(hWnd, &processId);

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,	FALSE, processId);

	if (processHandle)
	{
		BOOL ok;
		DWORD size;

		size = K32GetModuleFileNameExW(processHandle, NULL, buffer, ARRAYSIZE(buffer)); g_images[hWnd] = buffer;
		size = K32GetProcessImageFileNameW(processHandle, buffer, ARRAYSIZE(buffer)); g_images2[hWnd] = buffer;
		ok = QueryFullProcessImageNameW(processHandle, 0, buffer, &size); g_images3[hWnd] = buffer;
		ok = QueryFullProcessImageNameW(processHandle, 1, buffer, &size); g_images4[hWnd] = buffer;
	}

	CloseHandle(processHandle);

	std::wstringstream wss;
	wss << hWnd <<
		"\t[" << g_windows[hWnd].data() << "]" <<
		"\t[" << g_classes[hWnd].data() << "]" <<
		"\n2\t[" << g_images[hWnd].data() << "]" <<
		/*"\n3\t[" << g_images2[hWnd].data() << "]" <<
		"\n4\t[" << g_images3[hWnd].data() << "]" <<
		"\n5\t[" << g_images4[hWnd].data() << "]" <<*/
		"\n" << std::endl;

	wss.flush();
	OutputDebugStringW(wss.str().data());
	return TRUE;
}

int wmain(int argc, WCHAR* argv [])
{
	std::wcout << TEXT("Enumerating Windows...") << std::endl;
	BOOL enumeratingWindowsSucceeded = ::EnumWindows(enumWindowsProc, NULL);
	
	HWND hello = NULL;

	for (auto &wndp : g_windows)
	{
		if (wndp.second == L"Hello! - Internet Explorer")
			g_matched.push_back(wndp.first);
	}

	if (g_matched.empty()) return EXIT_SUCCESS;

	OutputDebugStringW(L"\nSending messages: \n");

	for (auto &wnd : g_matched)
	{
		OutputDebugStringW(g_classes[wnd].data());
		PostMessageA(wnd, WM_KEYDOWN, VK_LEFT, 0);
		PostMessageA(wnd, WM_KEYDOWN, VK_RIGHT, 0);
		PostMessageA(wnd, WM_KEYDOWN, VK_DOWN, 0);
		PostMessageA(wnd, WM_KEYDOWN, VK_UP, 0);
		OutputDebugStringW(L"\n");
	}

	
	
	std::cin.get();
	return EXIT_SUCCESS;
}

#else

void EnableCrt()
{
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
}

int DemoFaces1();
int DemoFaces2();
int DemoFaces3();
int DemoFaces4();
int DemoGestures1();
int DemoObjects1();

int wmain(int argc, WCHAR* argv [])
{
	EnableCrt();

	return DemoObjects1();
	//return DemoFaces1();
	//return DemoFaces2();
	//return DemoFaces3();
	//return DemoFaces4();
	//return DemoGestures1();
}

int DemoObjects1()
{
	using namespace Nena::Video;
	using namespace Nena::Video::Tracking;

	Capture::Status sts;

	Capture capture;
	Tracking::Object card(&capture);

	card.SearchProfile();					if (card.Failed()) return EXIT_FAILURE;
	card.SubmitRequirements(&capture);		if (card.Failed()) return EXIT_FAILURE;
	capture.LocateStreams();				if (capture.Failed()) return EXIT_FAILURE;



	auto sample = capture.GetSampleFromDesc(card.DescIndex);

	card.SetProfile(sample);						if (card.Failed()) return EXIT_FAILURE;



	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		PXCSmartArray<PXCImage> images;
		PXCSmartSP sp1, sp2;

		sts = sample->SubmitReadTask(images.ReleaseRefs(), &sp1);			//if (Utility::Failed(sts)) break;
		sts = card.SubmitProcessTask(images, &sp2);							//if (Utility::Failed(sts)) break;
		sp2->Synchronize();													//if (Utility::Failed(sts)) break;

		if (card.Tracker->GetTargetStatus(0) == PXCDCVTracker::STATUS_TRACKING)
		{
			wprintf(L"[Loop] Tracking [%s]\n", card.Tracker->GetTargetName(0));
		}

	}

	return EXIT_SUCCESS;


	return EXIT_SUCCESS;
}

int DemoFaces1()
{
	using namespace Nena::Video;
	using namespace Nena::Video::Tracking;

	pxcStatus sts;

	Capture capture;
	FacePxc face(&capture);

	face.SearchProfile(); if (!face.IsValid()) return EXIT_FAILURE;
	face.SearchLandmarkProfile(); if (!face.IsValid()) return EXIT_FAILURE;
	face.SearchDetectionProfile(); if (!face.IsValid()) return EXIT_FAILURE;

	face.SubmitRequirements(&capture); //if (!face.IsValid()) return EXIT_FAILURE;
	capture.LocateStreams(); if (!capture.IsValid()) return EXIT_FAILURE;

	face.SetProfile(); if (!face.IsValid()) return EXIT_FAILURE;
	face.SetLandmarkProfile(); if (!face.IsValid()) return EXIT_FAILURE;
	face.SetDetectionProfile(); if (!face.IsValid()) return EXIT_FAILURE;

	Sample *sample = capture.GetSampleFromDesc(face.DescIndex);

	UINT32 facesDetected = 0;

	PXCSmartSPArray sps(2);
	PXCSmartArray<PXCImage> images;

	char msg[64] = { 0 };

	while (!GetAsyncKeyState(VK_RETURN))
	{
		sts = sample->SubmitReadTask(images.ReleaseRefs(), sps.ReleaseRef(0)); if (Utility::Failed(sts)) break;
		sts = face.SubmitProcessTask(images, sps.ReleaseRef(1)); if (Utility::Failed(sts)) break;
		sts = sps.SynchronizeEx(); if (Utility::Failed(sts)) break;
		sts = sps[0]->Synchronize(0); if (Utility::Failed(sts)) break;

		facesDetected = face.GetLastFaceCount();
		Nena::Platform::DebugTrace("faces=%u\n", facesDetected);
	}

	return EXIT_SUCCESS;
}

int DemoFaces2()
{
	INT32 err = 0;

	if (FSDKE_OK != FSDK_ActivateLibrary("BDamqp/LmVRIFKBUehhOLhjFApxKwiISCu3sTBhksRWFAEpsHvuw7GG3Bs0Yf8QRnt7vo5MtOMhml0PD+ZPPm0G2UjVbfgCrm9Av448HWO0EROCkoGuSUJYnhguTluJcWwDAPo4n3zGlsmRHj3Aqo2Hmw9aK15ExvdQEoNNckYU="))
	{
		MessageBox(0, "Please run the License Key Wizard (Start - Luxand - FaceSDK - License Key Wizard)\n", "Error activating FaceSDK", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	err = FSDK_Initialize("");
	err = FSDK_InitializeCapturing();

	INT32 cameraCount;
	WCHAR **cameras;
	std::wstring strW;
	if (FSDKE_OK == FSDK_GetCameraList(&cameras, &cameraCount))
	for (int i = 0; i < cameraCount; i++) strW = cameras[i],
		Nena::Platform::DebugTrace("camera %d: %s\n", i, 
			std::string(strW.begin(), strW.end()).c_str()
			);

	if (0 == cameraCount)
	{
		MessageBox(0, "Please attach a camera", "Error", MB_ICONERROR | MB_OK);
		return -1;
	}

	INT32 cameraIndex = 1; // choose the first camera
	INT32 videoFormatCount = 0;
	FSDK_VideoFormatInfo * videoFormats = 0;

	err = FSDK_GetVideoFormatList(cameras[cameraIndex],
		&videoFormats, &videoFormatCount
		);

	for (INT32 i = 0; i < videoFormatCount; i++)
		Nena::Platform::DebugTrace("format %d: %dx%d, %d bpp\n", i,
			videoFormats[i].Width,videoFormats[i].Height, 
			videoFormats[i].BPP
			);

	INT32 videoFormat = 0; // choose a video format
	INT32 width = videoFormats[videoFormat].Width;
	INT32 height = videoFormats[videoFormat].Height;
	INT32 bitsPerPixel = videoFormats[videoFormat].BPP;

	err = FSDK_SetVideoFormat(cameras[cameraIndex],
		videoFormats[videoFormat]
		);

	Nena::Platform::DebugTrace("Trying to open the first camera...\n");

	INT32 cameraHandle = 0;
	if (FSDKE_OK != FSDK_OpenVideoCamera(cameras[cameraIndex], &cameraHandle))
	{
		MessageBox(0, "Error opening the first camera", "Error", MB_ICONERROR | MB_OK);
		return -2;
	}

	HTracker tracker = 0;
	err = FSDK_CreateTracker(&tracker);
	err = FSDK_SetTrackerMultipleParameters(
		tracker, 
		"DetermineFaceRotationAngle=false; "
		"HandleArbitraryRotations=false; "
		"FaceDetectionThreshold=5; "
		"InternalResizeWidth=100; "
		"RecognizeFaces=false; "
		"DetectGender=true; ", 
		&err
		);

	INT64 ids[256];
	INT64 faceCount = 0;
	CHAR attributes[1024];
	FLOAT male = 0.0f;
	FLOAT female = 0.0f;

	BOOL printOnNoneFound = TRUE;
	HImage imageHandle;
	TFacePosition facePosition;

	while (!GetAsyncKeyState(VK_RETURN))
		if (err = FSDK_GrabFrame(cameraHandle, &imageHandle) == FSDKE_OK)
		{
			if (err = FSDK_FeedFrame(tracker, 0, imageHandle, &faceCount, ids, sizeof(ids)) != FSDKE_OK) break;

			if (faceCount) Nena::Platform::DebugTrace("Faces: %d -> ", faceCount), printOnNoneFound = TRUE;
			else if (printOnNoneFound) Nena::Platform::DebugTrace("Cant see nothing...\n", faceCount), printOnNoneFound = FALSE;

			for (INT32 i = 0; i < faceCount; i++)
			{
				err = FSDK_GetTrackerFacePosition(tracker, cameraIndex, ids[i], &facePosition);
				err = FSDK_GetTrackerFacialAttribute(tracker, cameraIndex, ids[i], "Gender", attributes, sizeof(attributes));

				err = FSDK_GetValueConfidence(attributes, "Male", &male);
				err = FSDK_GetValueConfidence(attributes, "Female", &female);

				Nena::Platform::DebugTrace(" [%s, %d%%]", (male > female ? "Ma" : "Fe"),
					(male > female ? (INT32) (male * 100) : (INT32) (female * 100)));
			}

			if (faceCount) Nena::Platform::DebugTrace("\n");
			err = FSDK_FreeImage(imageHandle); // delete the FaceSDK image handle
		}

	err = FSDK_FreeTracker(tracker);
	err = FSDK_FreeVideoFormatList(videoFormats);
	err = FSDK_FreeCameraList(cameras, cameraCount);
	err = FSDK_FinalizeCapturing();
	err = FSDK_Finalize();

	return EXIT_SUCCESS;
}

int DemoFaces3()
{
	INT32 err = 0;

	if (FSDKE_OK != FSDK_ActivateLibrary("BDamqp/LmVRIFKBUehhOLhjFApxKwiISCu3sTBhksRWFAEpsHvuw7GG3Bs0Yf8QRnt7vo5MtOMhml0PD+ZPPm0G2UjVbfgCrm9Av448HWO0EROCkoGuSUJYnhguTluJcWwDAPo4n3zGlsmRHj3Aqo2Hmw9aK15ExvdQEoNNckYU="))
	{
		MessageBox(0, "Please run the License Key Wizard (Start - Luxand - FaceSDK - License Key Wizard)\n", "Error activating FaceSDK", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	err = FSDK_Initialize("");
	err = FSDK_InitializeCapturing();

	INT32 cameraCount;
	WCHAR **cameras;
	std::wstring strW;
	if (FSDKE_OK == FSDK_GetCameraList(&cameras, &cameraCount))
	for (int i = 0; i < cameraCount; i++) strW = cameras[i],
		Nena::Platform::DebugTrace("camera %d: %s\n", i,
		std::string(strW.begin(), strW.end()).c_str()
		);

	if (0 == cameraCount)
	{
		MessageBox(0, "Please attach a camera", "Error", MB_ICONERROR | MB_OK);
		return -1;
	}

	INT32 cameraIndex = 0; // choose the first camera
	INT32 videoFormatCount = 0;
	FSDK_VideoFormatInfo * videoFormats = 0;

	err = FSDK_GetVideoFormatList(cameras[cameraIndex],
		&videoFormats, &videoFormatCount
		);

	for (INT32 i = 0; i < videoFormatCount; i++)
		Nena::Platform::DebugTrace("format %d: %dx%d, %d bpp\n", i,
		videoFormats[i].Width, videoFormats[i].Height,
		videoFormats[i].BPP
		);

	INT32 videoFormat = 0; // choose a video format
	INT32 width = videoFormats[videoFormat].Width;
	INT32 height = videoFormats[videoFormat].Height;
	INT32 bitsPerPixel = videoFormats[videoFormat].BPP;

	err = FSDK_SetVideoFormat(cameras[cameraIndex],
		videoFormats[videoFormat]
		);

	Nena::Platform::DebugTrace("Trying to open the first camera...\n");

	INT32 cameraHandle = 0;
	if (FSDKE_OK != FSDK_OpenVideoCamera(cameras[cameraIndex], &cameraHandle))
	{
		MessageBox(0, "Error opening the first camera", "Error", MB_ICONERROR | MB_OK);
		return -2;
	}

	HTracker tracker = 0;
	err = FSDK_CreateTracker(&tracker);

	INT32 errPos;
	err = FSDK_SetTrackerMultipleParameters(
		tracker,
		"DetermineFaceRotationAngle=false; "
		"HandleArbitraryRotations=false; "
		"FaceDetectionThreshold=5; "
		"InternalResizeWidth=100;"
		/*"RecognizeFaces=false; "
		"DetectGender=true; "*/,
		&errPos
		);

	INT64 ids[256];
	INT64 faceCount = 0;
	//CHAR attributes[1024];
	FLOAT male = 0.0f;
	FLOAT female = 0.0f;

	BOOL printOnNoneFound = TRUE;
	HImage imageHandle;
	TFacePosition facePosition;

	//char userName[1024];

	while (!GetAsyncKeyState(VK_ESCAPE))
		if (err = FSDK_GrabFrame(cameraHandle, &imageHandle) == FSDKE_OK)
		{
			err = FSDK_FeedFrame(tracker, 0, imageHandle, &faceCount, ids, sizeof(ids));

			for (INT32 i = 0; i < faceCount; i++)
			{
				err = FSDK_GetTrackerFacePosition(tracker, cameraIndex, ids[i], &facePosition);

				char name[1024];
				int res = FSDK_GetAllNames(tracker, ids[i], name, sizeof(name));

				if (res == 0 && strlen(name) == 0 && 
					FSDK_LockID(tracker, ids[i]) == 0)
				{
					char inputName[64] = { 0 };
					std::cout << "Whats your name?\n";
					std::cin >> inputName;

					err = FSDK_SetName(tracker, ids[i], inputName);
					err = FSDK_UnlockID(tracker, ids[i]);
				}
				else
				{
					std::cout << "Hi! " << name << std::endl;
				}
			}

			err = FSDK_FreeImage(imageHandle); // delete the FaceSDK image handle
		}

	err = FSDK_FreeTracker(tracker);
	err = FSDK_FreeVideoFormatList(videoFormats);
	err = FSDK_FreeCameraList(cameras, cameraCount);
	err = FSDK_FinalizeCapturing();
	err = FSDK_Finalize();

	return EXIT_SUCCESS;

}

int DemoFaces4()
{
	using namespace Nena::Video;
	using namespace Nena::Video::Tracking;

	FaceLuxand::Status sts;

	std::cout << "Initializing, please, wait... (Press Esc to exit any moment)\n";

	FaceLuxand face;
	sts = face.QueryDevices();
	sts = face.SetDeviceByFormat();
	sts = face.Device->Open();

	auto tracker = face.QueryNewTracker();
	auto frame = tracker->QueryPerFrameData();

	INT errPos = -1;
	sts = tracker->SetMultipleParameters(FaceLuxand::Tracker::kName, &errPos);
	while (Utility::Succeeded(tracker->GrabFrame()) && !GetAsyncKeyState(VK_ESCAPE))
	{
		sts = tracker->FeedFrame(frame);

		if (frame->FacesFound) for (INT64 fidx = 0; fidx < frame->FacesFound && !sts; fidx++)
		{
			sts = frame->StoreFaceNamesByIdIndex(fidx);
			if (!strlen(frame->Names))
			{
				if (strcmp(frame->Names, "dunno"))
				{
					std::cout << "I cannot recognize you... Whats your name?\n";
					std::cin >> frame->Names;
					if (strlen(frame->Names)) frame->SetFaceNameByIdIndex(fidx);
				}
			}
			else
			{
				std::cout << "Hey! I know you! Hi, "<< frame->Names << "!\n";
			}

		}
		else
		{
			std::cout << "Hmm...\n";
		}

		sts = tracker->FreeFrame(frame);
	}

	std::cout << "Shutting down, please, wait...\n";
	sts = face.Device->Close();

	return EXIT_SUCCESS;
}

int DemoGestures1()
{
	using namespace Nena::Video;

	pxcStatus sts;

	Capture capture;
	Tracking::Gesture::DebugOutput eventsOutput;

	Tracking::Gesture gesture(&capture);		if (!gesture.IsValid()) return EXIT_FAILURE;

	gesture.SearchProfile();					if (!gesture.IsValid()) return EXIT_FAILURE;
	gesture.SubmitRequirements(&capture);		if (!gesture.IsValid()) return EXIT_FAILURE;
	capture.LocateStreams();					if (!capture.IsValid()) return EXIT_FAILURE;
	gesture.SetProfile();						if (!gesture.IsValid()) return EXIT_FAILURE;

	gesture.Subscribe(eventsOutput.GetGestureHandler());	if (!gesture.IsValid()) return EXIT_FAILURE;
	//gesture.Subscribe(eventsOutput.GetAlertHandler());		if (!gesture.IsValid()) return EXIT_FAILURE;

	auto sample = capture.GetSampleFromDesc(gesture.DescIndex);

	PXCSmartSPArray sps(2);
	PXCSmartArray<PXCImage> images;

	while (!GetAsyncKeyState(VK_RETURN))
	{
		sts = sample->SubmitReadTask(images.ReleaseRefs(), sps.ReleaseRef(0));		if (Utility::Failed(sts)) break;
		sts = gesture.SubmitProcessTask(images, sps.ReleaseRef(1));					if (Utility::Failed(sts)) break;
		sps.SynchronizeEx();														if (Utility::Failed(sts)) break;
	}

	return EXIT_SUCCESS;
}

#endif