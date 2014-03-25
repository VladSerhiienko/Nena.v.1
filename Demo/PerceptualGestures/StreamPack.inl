
template <pxcU32 _StreamCount>
inline pxcStatus PXCAPI Nena::Video::StreamPackSampler<_StreamCount>::ReadSync(PXCImage *images[], PXCScheduler::SyncPoint *spts[])
{
	OutputDebugStringA("Releasing sync points\n");
	if (spts[0]) spts[0]->SynchronizeEx(StreamPackHost::Count, Syncs);
	for (pxcU32 s = 0; s < StreamPackHost::Count; s++)
		spts[s]->Release();
	return PXC_STATUS_NO_ERROR;
}

template <pxcU32 _StreamCount>
inline pxcStatus PXCAPI Nena::Video::StreamPackSampler<_StreamCount>::PassOnStatus(pxcStatus sts)
{
	return sts;
}

template <pxcU32 _StreamCount>
inline Nena::Video::StreamPackSampler<_StreamCount>::StreamPackSampler(StreamPackHost *host) :
	Syncs(StreamPackHost::Count),
	Images(StreamPackHost::Count),
	Host(host),
	Sync()
{
}

template <pxcU32 _StreamCount>
inline void Nena::Video::StreamPackSampler<_StreamCount>::Close()
{
	Syncs[0] = nullptr;
	Syncs[1] = nullptr;

	Sync->Synchronize();
	Sync.ReleaseRef();
	Images.ReleaseRefs();
}

template <pxcU32 _StreamCount>
inline pxcStatus Nena::Video::StreamPackSampler<_StreamCount>::ReadAsync()
{
	typedef PXCSmartAsyncImplIxOy<ThisStreamPackSample, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT> AsyncImpl;

	pxcU32 streamIndex = 0;
	pxcStatus statuses[StreamPackHost::Count] = { PXC_STATUS_NO_ERROR };

	for (; streamIndex < StreamPackHost::Count; streamIndex++)
	{
		statuses[streamIndex] = Host->Array[streamIndex]->ReadStreamAsync(
			&Images[streamIndex], &Syncs[streamIndex]
			);

		if (statuses[streamIndex] < PXC_STATUS_NO_ERROR)
			return statuses[streamIndex];
	}

	OutputDebugStringA("Submiting async task\n");
	pxcStatus onSubmitTask = AsyncImpl::SubmitTask(
		Images, StreamPackHost::Count,
		Syncs, StreamPackHost::Count,
		&Sync, this, Host->Host->Scheduler,
		&ThisStreamPackSample::ReadSync,
		&ThisStreamPackSample::PassOnStatus,
		L"StreamPackSampler<>::ReadSync()"
		);

	return onSubmitTask;
}

template <pxcU32 _StreamCount>
inline Nena::Video::StreamPack<_StreamCount>::StreamPack(Nena::Video::Capture *capture) :
	Array(Count), Host(capture)
{
	ZeroMemory(&Desc, sizeof Desc);
	ZeroMemory(m_isVideoStreamInputSet, sizeof m_isVideoStreamInputSet);
}

template <pxcU32 _StreamCount>
inline pxcBool Nena::Video::StreamPack<_StreamCount>::AreProfilesSet()
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < Count; formatIndex++)
	if (!m_isVideoStreamInputSet[formatIndex])
		return FALSE;

	return TRUE;
}

template <pxcU32 _StreamCount>
inline void Nena::Video::StreamPack<_StreamCount>::SetProfiles(
	PXCSmartPtr<PXCCapture::VideoStream> &videoStream
	)
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < Count; formatIndex++)
	{
		for (PXCCapture::VideoStream::ProfileInfo &profile : ScannedProfiles[formatIndex])
		{
			if (profile.imageInfo.format == Desc.streams[formatIndex].format)
			{
				if (!m_isVideoStreamInputSet[formatIndex])
				{
					Profiles[formatIndex] = profile;
					videoStream->SetProfile(&Profiles[formatIndex]);
					Array[formatIndex] = videoStream.ReleasePtr();

					m_isVideoStreamInputSet[formatIndex] = TRUE;
				}
				else break;
			}
		}
	}
}

template <pxcU32 _StreamCount>
inline void Nena::Video::StreamPack<_StreamCount>::ScanProfiles(
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
		for (; formatIndex < Count; formatIndex++)
		{
			pxcU32 profileFormatMask =
				videoProfile.imageInfo.format & PXCImage::IMAGE_TYPE_MASK;
			pxcU32 requiredFormatMask =
				Desc.streams[formatIndex].format & PXCImage::IMAGE_TYPE_MASK;

			BOOL formatsMatch =
				profileFormatMask == requiredFormatMask;
			BOOL widthsMatch =
				(Desc.streams[formatIndex].sizeMin.width == 0 || Desc.streams[formatIndex].sizeMin.width <= videoProfile.imageInfo.width) &&
				(Desc.streams[formatIndex].sizeMax.width == 0 || Desc.streams[formatIndex].sizeMax.width >= videoProfile.imageInfo.width);
			BOOL heightsMatch =
				(Desc.streams[formatIndex].sizeMin.height == 0 || Desc.streams[formatIndex].sizeMin.height <= videoProfile.imageInfo.height) &&
				(Desc.streams[formatIndex].sizeMax.height == 0 || Desc.streams[formatIndex].sizeMax.height >= videoProfile.imageInfo.height);

			if (formatsMatch && widthsMatch && heightsMatch)
				ScannedProfiles[formatIndex].push_back(videoProfile);
		}
	}
}


template <pxcU32 _StreamCount>
inline HRESULT Nena::Video::StreamPack<_StreamCount>::Init(void)
{
	HRESULT result = E_FAIL;

	if (!Host) return result;
	else if (!Host->Device) return result;
	else result = S_OK;

	pxcStatus status = PXC_STATUS_NO_ERROR;

	pxcU32 streamsRequired = GetFormatsCount();

	pxcU32 streamIndex = 0;
	pxcBool isVideoStreamFound = FALSE;
	for (; (status >= PXC_STATUS_NO_ERROR) & (!isVideoStreamFound); streamIndex++)
	{
		PXCCapture::Device::StreamInfo Info;
		status = Host->Device->QueryStream(streamIndex, &Info);

		if (status < PXC_STATUS_NO_ERROR) break; else
		if (Info.cuid != PXCCapture::VideoStream::CUID) continue;

		PXCSmartPtr<PXCCapture::VideoStream> videoStream;
		status = Host->Device->CreateStream<PXCCapture::VideoStream>(
			streamIndex, videoStream.ReleaseRef()
			);

		ScanProfiles(videoStream);
		SetProfiles(videoStream);

		if (AreProfilesSet())
			break;
	}

	if (status < PXC_STATUS_NO_ERROR) result = E_FAIL;
	return result;

}



template <pxcU32 _StreamCount>
inline void Nena::Video::StreamPack<_StreamCount>::Close()
{
	Array[0] = nullptr;
	Array[1] = nullptr;
	Array.ReleaseRefs();
}

template <pxcU32 _StreamCount>
inline void Nena::Video::StreamPack<_StreamCount>::Frame()
{
	PXCImage *image;
	PXCSmartSP sp;
	Array[0]->ReadStreamAsync(&image, &sp);

	//PXCImage::SURFACE_TYPE_DX11_SURFACE

	PXCImage::ImageData data;
	image->AcquireAccess(PXCImage::ACCESS_READ, &data);
}

template <pxcU32 _StreamCount>
inline pxcU32 Nena::Video::StreamPack<_StreamCount>::GetFormatsCount()
{
	pxcU32 formatIndex = 0;
	for (; formatIndex < Count; formatIndex++)
	{
		if (!Desc.streams[formatIndex].format)
			break;
	}
	return formatIndex;
}