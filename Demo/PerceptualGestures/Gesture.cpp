
#include "Gesture.h"
#include "Capture.h"
#include "Pipe.h"

void Nena::Video::Processing::Gesture::MissingOnGestureCallback(
	Gesture *inst, PXCGesture::Gesture *data
	)
{
	static WCHAR s_msg[128] = { 0 };
	OutputDebugStringA("MissingOnGestureCallback()\n");

	if (data->label == PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT)
		wsprintfW(s_msg, L"Swipe left id = %x\n", data->label),
		OutputDebugStringW(s_msg);
	if (data->label == PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT)
		wsprintfW(s_msg, L"Swipe right id = %x\n", data->label),
		OutputDebugStringW(s_msg);
	if (data->label == PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN)
		wsprintfW(s_msg, L"Swipe down id = %x\n", data->label),
		OutputDebugStringW(s_msg);
	if (data->label == PXCGesture::Gesture::LABEL_NAV_SWIPE_UP)
		wsprintfW(s_msg, L"Swipe up id = %x\n", data->label),
		OutputDebugStringW(s_msg);
}

void Nena::Video::Processing::Gesture::MissingOnAlertCallback(
	Gesture *inst, PXCGesture::Alert *data
	)
{
	static WCHAR s_msg[128] = { 0 };
	OutputDebugStringA("MissingOnAlertCallback()\n");

	if (data->label == PXCGesture::Alert::LABEL_GEONODE_ACTIVE)
		wsprintfW(s_msg, L"Found hand id = %x\n", data->label),
		OutputDebugStringW(s_msg);
	if (data->label == PXCGesture::Alert::LABEL_GEONODE_INACTIVE)
		wsprintfW(s_msg, L"Lose hand id = %x\n", data->label),
		OutputDebugStringW(s_msg);
}


HRESULT Nena::Video::Processing::Gesture::MissingOnProcessCallback(
	Gesture *inst
	)
{
	OutputDebugStringA("processing...\n");


	PXCGesture::GeoNode hand_data[5];

	pxcStatus status = inst->Base->QueryNodeData(
		0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY | PXCGesture::GeoNode::LABEL_FINGER_THUMB, 
		5, hand_data
		);

	if (status == PXC_STATUS_NO_ERROR)
	{
		for (uint32_t i = 0; i < 5; i++)
		{
			Platform::DebugTrace(
				"\t\tFinger[%u] X=%f Y=%f Z=%f\n",
				i, hand_data[i].positionWorld.x,
				hand_data[i].positionWorld.y,
				hand_data[i].positionWorld.z
				);
		}
	}
	else
	{
		/*Platform::DebugTrace(
			"\t\t0x%08x\n", status
			);*/
	}

	return S_OK;
}

void Nena::Video::Processing::Gesture::SetProfile(
	Gesture::ProfileInfo *profile // can be null
	)
{
	pxcStatus status = profile
		? PXC_STATUS_NO_ERROR // we can use this profile
		: PXC_STATUS_ITEM_UNAVAILABLE; // try to get the default profile

	// ensure this instance was initialized
	if (!profile && Base.IsValid())
	{
		UINT32 gestureProfileIndex = 0;
		// query default profile
		status = Base->QueryProfile(
			gestureProfileIndex,
			&Profile
			);
		// this operation can fail, so do the check
		if (status >= PXC_STATUS_NO_ERROR)
			// push to scanned list so that we could then see the default profile at the head
			ScannedProfiles.push_front(Profile),
			// set pointer to initialize gesture base corrently in case null pointer was passed
			profile = &Profile;
	}

	// complete operation if everything went ok
	if (status >= PXC_STATUS_NO_ERROR)
		status = Base->SetProfile(profile);
}

Nena::Video::Processing::Gesture::Gesture(Streamer &host) :
Gesture::Gesture(&host)
{
}

Nena::Video::Processing::Gesture::Gesture(Streamer *host) :
	Host(host), Base(nullptr), ScannedProfiles(),
	OnProcessTrackingResults(&MissingOnProcessCallback),
	OnGestureCall(&MissingOnGestureCallback),
	OnAlertCall(&MissingOnAlertCallback),
	Sampler(host)
{
	HRESULT result = E_POINTER;
	pxcStatus status = PXC_STATUS_NO_ERROR;

	ZeroMemory(&Desc, sizeof Desc);
	ZeroMemory(&Profile, sizeof Profile);

	// try to create gesture base in ctor

	if (Host)
		//if (!Host->AreProfilesSet())
	if (Host->Host) if (Host->Host->Host)
	if (Host->Host->Host->Session.IsValid())
	{
		status = Host->Host->Host->Session->CreateImpl<PXCGesture>(
			&Desc, Base.ReleaseRef()
			);

		result = status < PXC_STATUS_NO_ERROR
			? (Platform::DebugTrace("Failed to create gesture base impl\n"), E_FAIL)
			: S_OK;
	}

	// use "result" and "status" to handle errors
	// - if smth was null, then "result == E_POINTER"
	// - if smth was failed, then "result == E_FAIL"
	// - "status" will show the real reason if smth else had occured

}

//Nena::Video::Processing::Gesture::~Gesture()
//{
//	Base = nullptr;
//}

void PXCAPI Nena::Video::Processing::Gesture::OnGesture(PXCGesture::Gesture *gesture)
{
	return OnGestureCall(this, gesture);
}

void PXCAPI Nena::Video::Processing::Gesture::OnAlert(PXCGesture::Alert *alert)
{
	return OnAlertCall(this, alert);
}

HRESULT Nena::Video::Processing::Gesture::Init(BOOL recreateGestureBase)
{
	pxcStatus status = PXC_STATUS_NO_ERROR;

	// create if it is still null (after ctor) or need to be recreated
	if (!Base.IsValid() || recreateGestureBase)
	if (!Host) { Platform::DebugTrace("Streamer instance is null\n"); return E_HANDLE; }
	//else if (!Host->AreProfilesSet()) { Platform::DebugTrace("Set streamer profiles\n"); return E_HANDLE; }
	else if (!Host->Host) { Platform::DebugTrace("Capture instance is null\n"); return E_HANDLE; }
	else if (!Host->Host->Host) { Platform::DebugTrace("Pipe instance is null\n"); return E_HANDLE; }
	else if (!Host->Host->Host->Session.IsValid()) { Platform::DebugTrace("Session cannot be null\n"); return E_HANDLE; }
	else status = Host->Host->Host->Session->CreateImpl<PXCGesture>(
		Base.ReleaseRef()
		);

	// return operation results
	if (status < PXC_STATUS_NO_ERROR || !Base.IsValid())
		return E_FAIL;
	return S_OK;
}

void Nena::Video::Processing::Gesture::Close()
{
	//Base = nullptr;
	Base.ReleaseRef();
	CleanProfiles();
}

HRESULT Nena::Video::Processing::Gesture::LocateStreams()
{
	HRESULT result = Host
		? S_OK : E_POINTER;

	// here should be at least one profile at the head
	if (ScannedProfiles.empty())
		return E_FAIL;

	if (Host)
		Host->SetDescription(Profile.inputs),
		result = Host->Init()/*,
		Sample = Sampler.AllocNewSample()*/;

	return result;
}

void Nena::Video::Processing::Gesture::CleanProfiles()
{
	ScannedProfiles.clear();
}

void Nena::Video::Processing::Gesture::ScanProfiles()
{
	pxcStatus status;
	UINT32 gestureProfileIndex = 0;
	PXCGesture::ProfileInfo gestureProfile;

	// try to get all profiles it can query
	// then we can choose the one we need
	for (;; gestureProfileIndex++)
	{
		status = Base->QueryProfile(
			gestureProfileIndex,
			&gestureProfile
			);

		if (status >= PXC_STATUS_NO_ERROR)
			ScannedProfiles.push_front(
			gestureProfile
			);
		else
			break;
	}
}

UINT32 Nena::Video::Processing::Gesture::GetRequiredStreamCount()
{
	return Streamer::GetFormatsCount(
		Profile.inputs
		);
}

HRESULT Nena::Video::Processing::Gesture::SubscribeGesture()
{
	if (Profile.sets && Base.IsValid())
		return Base->SubscribeGesture(ThresholdMax, this) < PXC_STATUS_NO_ERROR
		? E_FAIL : S_OK;
	else
		return E_INVALIDARG;
}

HRESULT Nena::Video::Processing::Gesture::SubscribeAlert()
{
	if (Profile.alerts && Base.IsValid())
		return Base->SubscribeAlert(this) < PXC_STATUS_NO_ERROR
		? E_FAIL : S_OK;
	else
		return E_INVALIDARG;
}

HRESULT Nena::Video::Processing::Gesture::UnsubscribeGesture()
{
	if (Base.IsValid())
		return Base->SubscribeGesture(ThresholdMin, nullptr) < PXC_STATUS_NO_ERROR
		? E_FAIL : S_OK;
	else
		return E_INVALIDARG;
}

HRESULT Nena::Video::Processing::Gesture::UnsubscribeAlert()
{
	if (Base.IsValid())
		return Base->SubscribeAlert(nullptr) < PXC_STATUS_NO_ERROR
		? E_FAIL : S_OK;
	else
		return E_INVALIDARG;
}

HRESULT Nena::Video::Processing::Gesture::OnFrameMove()
{
	//OutputDebugStringA("OnFrameMove()\n ");

	PXCSmartSP sync;

	pxcStatus statusOnProcess = PXC_STATUS_NO_ERROR,
		statusOnSync = PXC_STATUS_NO_ERROR;
	HRESULT resultOnSubmit = S_OK,
		resultOnAwait = S_OK,
		resultOnProcess = S_OK;

	PXCSmartArray<PXCImage> images; // (Streamer::MaxStreamsPerInstance);
	//PXCCapture::VideoStream::Images images = { NULL }; // (Streamer::MaxStreamsPerInstance);

	PXCSmartSPArray syncs(2); // (Streamer::MaxStreamsPerInstance);

	resultOnSubmit = Sampler.SubmitReadTask(images, syncs.ReleaseRef(0));
	/*statusOnSync = syncs[0]->Synchronize();
	OutputDebugStringA("synced 0...\n");*/

	statusOnProcess = Base->ProcessImageAsync(images, syncs.ReleaseRef(1));

	if (statusOnProcess == PXC_STATUS_NO_ERROR)
	{
		/*Platform::DebugTrace(
			"&images[0] = 0x%08x &images[1] = 0x%08x &sync[0] = 0x%08x &sync[1] = 0x%08x\n"
			" images[0] = 0x%08x  images[1] = 0x%08x  sync[0] = 0x%08x  sync[1] = 0x%08x\n",
			&images[0], &images[1], &syncs[0], &syncs[1],
			images[0], images[1], syncs[0], syncs[1]
			);
		Platform::DebugTrace(
			"resultOnSubmit=0x%08x statusOnProcess=0x%08x \n",
			resultOnSubmit, statusOnProcess
			);*/

		/*statusOnSync = syncs[1]->Synchronize();
		OutputDebugStringA("synced 1...\n");*/

		syncs.SynchronizeEx();
		//statusOnSync = syncs[0]->Synchronize(0);

		resultOnProcess = OnProcessTrackingResults(this);
	}
	else
	{
		Platform::DebugTrace(
			"ProcessImageAsync() failed\n",
			resultOnSubmit, statusOnProcess
			);

	}


	return resultOnProcess;
}

