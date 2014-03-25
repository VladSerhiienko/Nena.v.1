#include "Streamer.h"
#include "Capture.h"

Nena::Video::Sampler::Sample::Sample() :
Data(nullptr), /*Info(nullptr),*/
Size(0), State(nullptr)
{
}

Nena::Video::Sampler::Sample::~Sample()
{
	Delete();
}

void Nena::Video::Sampler::Sample::Delete()
{
	delete [] LastErrors;
	LastErrors = nullptr;

	DeleteData();
	/*DeleteInfo();*/
}

void Nena::Video::Sampler::Sample::DeleteData()
{

	delete [] Data;
	Data = nullptr;
}

//void Nena::Video::Sampler::Sample::DeleteInfo()
//{
//	delete [] Info; Info = nullptr;
//}

void Nena::Video::Sampler::Sample::Init(Sampler *sampler, UINT32 size)
{
	Size = size;
	LastErrors = new Status[size];
	Data = new PXCImage::ImageData[size];
	//ZeroMemory(Data, (sizeof PXCImage::ImageData) * size);

	/*if (info)
	{
	Info = new PXCImage::ImageInfo[size];
	ZeroMemory(Info, (sizeof PXCImage::ImageInfo) * size);
	}
	else Info = nullptr;*/
}

pxcStatus PXCAPI Nena::Video::Sampler::ReadSync(PXCImage *images [], PXCScheduler::SyncPoint *spts [])
{
	Platform::DebugTrace("\t Sampler::ReadSync() [Size=%u]\n", Host->SizeU());
	if (spts[0]) spts[0]->SynchronizeEx(Host->SizeU(), spts); // synchronize
	for (pxcU32 s = 0; s < Host->SizeU(); s++) if (spts[s]) spts[s]->Release(); // release
	Platform::DebugTrace("\t Sampler::ReadSync() ok\n", Host->SizeU());

	return PXC_STATUS_NO_ERROR; // ok
}

pxcStatus PXCAPI Nena::Video::Sampler::PassOnStatus(pxcStatus sts)
{
	Platform::DebugTrace("\t Sampler::PassOnStatus() [Status=%u]\n", sts);

	return sts;
}

Nena::Video::Sampler::Sampler(Streamer *host) :
Images(host ? host->SizeU() : Streamer::MaxStreamsPerInstance),
Syncs(host ? host->SizeU() : Streamer::MaxStreamsPerInstance),
Host(host), Sync()
{
}

Nena::Video::Sampler::Sampler(Streamer &host) :
Images(host.SizeU()),
Syncs(host.SizeU()),
Host(&host), Sync()
{
}

Nena::Video::Sampler::Sample * Nena::Video::Sampler::AllocNewSample()
{
	if (!Host) { Platform::DebugTrace("Stramer instance is null\n"); return nullptr; }
	else if (!Host->AreProfilesSet()) { Platform::DebugTrace("Set streamer profiles\n"); return nullptr; }

	Sample *newSample = new Sample();
	newSample->Init(this, Host->SizeU());

	m_allocatedSamples.push_front(newSample);
	return newSample;
}

void Nena::Video::Sampler::Close()
{
	for (auto &sample : m_allocatedSamples) delete sample;
	m_allocatedSamples.clear();

	pxcU32 syncIndex = 0;
	pxcU32 syncCount = (pxcU32) Syncs.QuerySize();
	for (; syncIndex < syncCount;
		Syncs[syncIndex] = NULL,
		syncIndex++
		);

	//Sync->Synchronize();
	Sync.ReleaseRef();
	Images.ReleaseRefs();
}

//Nena::Video::Sampler::Status Nena::Video::Sampler::QueryImagesInfo(Sample *sample)
//{
//	UINT32 imageIndex = 0;
//	for (; imageIndex < sample->Size; imageIndex++)
//		sample->LastErrors[imageIndex] =
//			Images[imageIndex]->QueryInfo(
//				sample->Info + imageIndex
//				);
//}

Nena::Video::Sampler::Action Nena::Video::Sampler::AcquireReadAccess(Sample *sample)
{
	UINT32 imageIndex = 0;
	for (; imageIndex < sample->Size; imageIndex++)
		sample->LastErrors[imageIndex] =
		Images[imageIndex]->AcquireAccess(
		PXCImage::ACCESS_READ,
		sample->Data + imageIndex
		);
}

Nena::Video::Sampler::Action Nena::Video::Sampler::AcquireReadAccess(Sample *sample, UINT32 imageIndex)
{
	sample->LastErrors[imageIndex] =
		Images[imageIndex]->AcquireAccess(
		PXCImage::ACCESS_READ,
		sample->Data + imageIndex
		);
}

Nena::Video::Sampler::Action Nena::Video::Sampler::ReleaseAccess(Sample *sample)
{
	UINT32 imageIndex = 0;
	for (; imageIndex < sample->Size; imageIndex++)
		sample->LastErrors[imageIndex] =
		Images[imageIndex]->ReleaseAccess(
		sample->Data + imageIndex
		);
}

Nena::Video::Sampler::Action Nena::Video::Sampler::ReleaseAccess(Sample *sample, UINT32 imageIndex)
{
	sample->LastErrors[imageIndex] =
		Images[imageIndex]->ReleaseAccess(
		sample->Data + imageIndex
		);
}

//Nena::Video::Sampler::StatusAsync Nena::Video::Sampler::QueryImagesInfoAsync(Sample *sample)
//{
//	return concurrency::create_task([this, sample] { return QueryImagesInfo(sample); });
//}

Nena::Video::Sampler::Status Nena::Video::Sampler::SubmitReadTask(
	_Out_ PXCImage *imagesOut [], _Out_ PXCScheduler::SyncPoint **syncOut
	)
{
	typedef PXCSmartAsyncImplIxOy<Sampler, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT,
		PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT> AsyncImpl;

	if (Host->SizeU() == 1u)
	{
		return  Host->Streams[0]->ReadStreamAsync(
			_In_ imagesOut, _Out_ syncOut
			);
	}

	pxcStatus statuses[Streamer::MaxStreamsPerInstance] = { PXC_STATUS_NO_ERROR };

	PXCSmartSPArray syncsIn(Streamer::MaxStreamsPerInstance);

	pxcU32 streamIndex = 0;
	for (; streamIndex < Host->SizeU(); streamIndex++)
	{
		statuses[streamIndex] = Host->Streams[streamIndex]->ReadStreamAsync(
			_In_ &imagesOut[streamIndex], _Out_ &syncsIn[streamIndex]
			);

		if (statuses[streamIndex] < PXC_STATUS_NO_ERROR)
			return statuses[streamIndex];
	}

	//OutputDebugStringA("Submiting async task\n");
	pxcStatus onSubmitTask = AsyncImpl::SubmitTask(
		imagesOut, Host->SizeU(),
		syncsIn.ReleasePtrs(), Host->SizeU(),
		syncOut, this, Host->Host->Scheduler,
		&Sampler::ReadSync,
		&Sampler::PassOnStatus,
		//L"Nena::Video::Sampler::ReadSync()"
		nullptr
		//L""
		);

	return onSubmitTask;

}


Nena::Video::Sampler::Status Nena::Video::Sampler::SubmitReadTask()
{
	typedef PXCSmartAsyncImplIxOy<Sampler, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, 
		PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT> AsyncImpl;

	if (Host->SizeU() == 1u)
	{
		return  Host->Streams[0]->ReadStreamAsync(
			_In_ Images, _Out_ &Sync
			);
	}

	pxcStatus statuses[Streamer::MaxStreamsPerInstance] = { PXC_STATUS_NO_ERROR };

	pxcU32 streamIndex = 0;
	for (; streamIndex < Host->SizeU(); streamIndex++)
	{
		statuses[streamIndex] = Host->Streams[streamIndex]->ReadStreamAsync(
			_In_ &Images[streamIndex], _Out_ &Syncs[streamIndex]
			);

		if (statuses[streamIndex] < PXC_STATUS_NO_ERROR)
			return statuses[streamIndex];
	}

	//OutputDebugStringA("Submiting async task\n");
	pxcStatus onSubmitTask = AsyncImpl::SubmitTask(
		Images, Host->SizeU(),
		Syncs.ReleasePtrs(), Host->SizeU(),
		&Sync, this, Host->Host->Scheduler,
		&Sampler::ReadSync,
		&Sampler::PassOnStatus,
		//L"StreamPackSampler<>::ReadSync()"
		nullptr
		//L""
		);

	return onSubmitTask;
}

//Nena::Video::Streamer::~Streamer() { Close(); }

Nena::Video::Streamer::Streamer(Capture *capture) :
Streamer::Streamer(Streamer::MaxStreamsPerInstance, capture)
{
}

Nena::Video::Streamer::Streamer(pxcU32 streamCount, Capture *capture) :
Host(capture),
Streams(streamCount),
m_actualStreamCount(streamCount)
{
	ZeroMemory(&Desc, sizeof Desc);
	ZeroMemory(m_isVideoStreamInputSet, sizeof m_isVideoStreamInputSet);

}

pxcBool Nena::Video::Streamer::AreProfilesSet()
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < SizeU(); formatIndex++)
	if (!m_isVideoStreamInputSet[formatIndex])
		return FALSE;

	return TRUE;
}

void Nena::Video::Streamer::SetProfiles(
	PXCSmartPtr<PXCCapture::VideoStream> &videoStream
	)
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < SizeU(); formatIndex++)
	{
		for (auto &profilePair : ScannedProfiles) if (profilePair.first == formatIndex)
		{
			PXCCapture::VideoStream::ProfileInfo profile = profilePair.second;
			if (profile.imageInfo.format == Desc.streams[formatIndex].format)
			{
				if (!m_isVideoStreamInputSet[formatIndex])
				{
					MatchedProfiles[formatIndex] = profile;
					videoStream->SetProfile(&MatchedProfiles[formatIndex]);
					Streams[formatIndex] = videoStream.ReleasePtr();

					m_isVideoStreamInputSet[formatIndex] = TRUE;
				}
				else break;
			}
		}
	}
}

void Nena::Video::Streamer::ScanProfiles(
	PXCSmartPtr<PXCCapture::VideoStream> &videoStream
	)
{
	pxcU16 profileIndex = 0;
	for (;; profileIndex++)
	{
		PXCCapture::VideoStream::ProfileInfo videoProfile;
		pxcStatus status = videoStream->QueryProfile(
			profileIndex, &videoProfile
			);

		if (status < PXC_STATUS_NO_ERROR) break;

		pxcBool goodFormat = FALSE;
		pxcU32 formatIndex = 0;
		for (; formatIndex < SizeU(); formatIndex++)
		{
			pxcU32 profileFormatMask =
				videoProfile.imageInfo.format & PXCImage::IMAGE_TYPE_MASK;
			pxcU32 requiredFormatMask =
				Desc.streams[formatIndex].format & PXCImage::IMAGE_TYPE_MASK;

			BOOL formatsMatch =
				profileFormatMask == requiredFormatMask;
			BOOL widthsMatch =
				(Desc.streams[formatIndex].sizeMin.width == 0 || 
				Desc.streams[formatIndex].sizeMin.width <= videoProfile.imageInfo.width) &&
				(Desc.streams[formatIndex].sizeMax.width == 0 || 
				Desc.streams[formatIndex].sizeMax.width >= videoProfile.imageInfo.width);
			BOOL heightsMatch =
				(Desc.streams[formatIndex].sizeMin.height == 0 || 
				Desc.streams[formatIndex].sizeMin.height <= videoProfile.imageInfo.height) &&
				(Desc.streams[formatIndex].sizeMax.height == 0 || 
				Desc.streams[formatIndex].sizeMax.height >= videoProfile.imageInfo.height);

			if (formatsMatch && widthsMatch && heightsMatch)
				ScannedProfiles.insert(std::make_pair(
				formatIndex, videoProfile
				));
		}
	}
}

HRESULT Nena::Video::Streamer::Init(void)
{
	HRESULT result = E_FAIL;

	if (!Host) { Platform::DebugTrace("Capture instance is null\n"); return result; }
	else if (!Host->Device) { Platform::DebugTrace("Capture device instance is null\n"); return result; }
	else result = S_OK;

	pxcU32 streamsRequired =
		m_actualStreamCount =
		GetFormatsCount();

	pxcU32 streamIndex = 0;
	pxcBool isVideoStreamFound = FALSE;
	pxcStatus status = PXC_STATUS_NO_ERROR;

	for (; (status >= PXC_STATUS_NO_ERROR) &
		(!isVideoStreamFound);
		streamIndex++)
	{
		PXCCapture::Device::StreamInfo Info;
		status = Host->Device->QueryStream(streamIndex, &Info);

		if (status < PXC_STATUS_NO_ERROR) break; else
		if (Info.cuid != PXCCapture::VideoStream::CUID) continue;

		PXCSmartPtr<PXCCapture::VideoStream> videoStream;
		status = Host->Device->CreateStream/*<PXCCapture::VideoStream>*/(
			streamIndex, PXCCapture::VideoStream::CUID, (void**)&videoStream
			);

		ScanProfiles(videoStream);
		SetProfiles(videoStream);

		if (AreProfilesSet())
			break;
	}

	if (status < PXC_STATUS_NO_ERROR) result = E_FAIL;
	return result;

}

pxcU32 Nena::Video::Streamer::GetFormatsCount(
	PXCCapture::VideoStream::DataDesc &desc
	)
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < Streamer::MaxStreamsPerInstance; formatIndex++)
	if (!desc.streams[formatIndex].format)
		break;

	return formatIndex;
}

pxcU32 Nena::Video::Streamer::GetFormatsCount()
{
	/*pxcU32 formatIndex = 0;
	for (; formatIndex < SizeU(); formatIndex++)
	if (!Desc.streams[formatIndex].format)
	break;

	return formatIndex;*/
	return GetFormatsCount(Desc);
}

void Nena::Video::Streamer::Close()
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < SizeU();
		Streams[formatIndex] = NULL,
		formatIndex++
		);

	Streams.ReleaseRefs();
}

void Nena::Video::Streamer::SetDescription(
	PXCCapture::VideoStream::DataDesc &desc
	)
{
	memcpy(&Desc, &desc, sizeof Desc);
	m_actualStreamCount = GetFormatsCount();
}

void Nena::Video::Streamer::SetDescription(
	PXCCapture::VideoStream::DataDesc *desc
	)
{
	if (desc)
		memcpy(&Desc, &desc, sizeof Desc),
		m_actualStreamCount = GetFormatsCount();
}

Nena::Video::Sampler::ActionAsync Nena::Video::Sampler::AcquireReadAccessAsync(Sample *sample)
{
	return concurrency::create_task([this, sample] { return AcquireReadAccess(sample); });
}

Nena::Video::Sampler::ActionAsync Nena::Video::Sampler::AcquireReadAccessAsync(Sample *sample, UINT32 imageIndex)
{
	return concurrency::create_task([this, sample, imageIndex] { return AcquireReadAccess(sample, imageIndex); });
}

Nena::Video::Sampler::ActionAsync Nena::Video::Sampler::ReleaseAccessAsync(Sample *sample)
{
	return concurrency::create_task([this, sample] { return ReleaseAccess(sample); });
}

Nena::Video::Sampler::ActionAsync Nena::Video::Sampler::ReleaseAccessAsync(Sample *sample, UINT32 imageIndex)
{
	return concurrency::create_task([this, sample, imageIndex] { return ReleaseAccess(sample, imageIndex); });
}

Nena::Video::Sampler::Status Nena::Video::Sampler::AwaitReadTask()
{
	Nena::Video::Sampler::Status status = Sync->Synchronize(); Sync.ReleaseRef();
	return status;
}

Nena::Video::Sampler::StatusAsync Nena::Video::Sampler::AwaitReadTaskAsync()
{
	return concurrency::create_task([this] { return AwaitReadTask(); });
}

Nena::Video::Sampler::StatusAsync Nena::Video::Sampler::SubmitReadTaskAsync()
{
	return concurrency::create_task([this] { return SubmitReadTask(); });
}