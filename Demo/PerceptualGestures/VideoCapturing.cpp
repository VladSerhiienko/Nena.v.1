#include "Video.h"

namespace Nena
{
	namespace Video
	{

		UINT32 Capture::CalculateStreamCount(VideoStreamDataDesc &desc)
		{
			return CalculateStreamCount(&desc);
		}

		UINT32 Capture::CalculateStreamCount(VideoStreamDataDescVector &desc)
		{
			return CalculateStreamCount(&desc);
		}

		UINT32 Capture::CalculateStreamCount(VideoStreamDataDesc *desc)
		{
			UINT32 i = 0;
			for (; desc && desc->streams[i].format && i < Sample::MaxStreams; i++);
			return i;
		}

		UINT32 Capture::CalculateStreamCount(VideoStreamDataDescVector *desc)
		{
			UINT32 i = 0;
			if (desc)
			for (auto item = desc->begin(); item != desc->end(); item++)
				i += CalculateStreamCount(*item);
			return i;
		}

		void Capture::DeleteSamples()
		{
			for (auto &sample : Samples)
				delete sample;
			Samples.clear();
		}

		void Capture::Clear()
		{
			VideoMapping.clear();

			VideoMapping.resize(VideoDescs.size());
			for (auto &set : VideoMapping)
			for (auto &s : set.u32)
				s = UINT_ERROR;
		}

		Capture::~Capture()
		{
			DeleteSamples();
			//DeleteStreams();
		}

		Capture::Capture(PXCSession *session) :
			Utility::BasicStatus<>(),
			Session(session)
		{
				Utility::GenericZeroMemory(&DeviceFilter);
				Utility::GenericZeroMemory(&DescFilter);

				if (!Session.IsValid())
					Check() = PXCSession_Create(
					Session.ReleaseRef()
					);
				if (!Session.IsValid()) return;
				else Check() = Session->CreateScheduler(
					Scheduler.ReleaseRef()
					);
			}

		Capture::Status Capture::CreateBase(UINT32 profileIndex, PXCCapture **capture)
		{
			PXCSession::ImplDesc desc;
			Check() = Session->QueryImpl(&DescFilter, profileIndex, &desc);
			if (Failed()) return PXC_STATUS_ITEM_UNAVAILABLE;
			return Session->CreateImpl<PXCCapture>(&desc, capture);
		}

		void Capture::ScanProfiles(
			_Out_ VideoStreamProfileList &profiles,
			_In_ PXCCapture::VideoStream *videoStream
			)
		{
			for (int profileIndex = 0;; profileIndex++)
			{
				PXCCapture::VideoStream::ProfileInfo profileInfo;
				Check() = videoStream->QueryProfile(profileIndex, &profileInfo);
				if (Failed()) break;

				if (VideoFilters.empty()) profiles.push_back(profileInfo); else
				{
					BOOL filtered = FALSE, ok = FALSE;
					VideoStreamProfileList::iterator profileBox = VideoFilters.begin();
					for (; profileBox != VideoFilters.end(); profileBox++)
					{
						if ((profileBox->imageInfo.format & PXCImage::IMAGE_TYPE_MASK) !=
							(profileInfo.imageInfo.format & PXCImage::IMAGE_TYPE_MASK))
							continue;

						filtered = TRUE;

						if (profileBox->imageInfo.width &&
							profileBox->imageInfo.width != profileInfo.imageInfo.width)
							continue;

						if (profileBox->imageInfo.height &&
							profileBox->imageInfo.height != profileInfo.imageInfo.height)
							continue;

						FLOAT fmin = (FLOAT) profileBox->frameRateMin.numerator / profileBox->frameRateMin.denominator;
						FLOAT fmax = (FLOAT) profileBox->frameRateMax.numerator / profileBox->frameRateMax.denominator;
						FLOAT fthismin = (FLOAT) profileInfo.frameRateMin.numerator / profileInfo.frameRateMin.denominator;
						FLOAT fthismax = (FLOAT) profileInfo.frameRateMax.numerator / profileInfo.frameRateMax.denominator;

						if (profileBox->frameRateMin.denominator)
						{
							if (profileInfo.frameRateMin.denominator) if (fmin > fthismin) continue;
							if (profileInfo.frameRateMax.denominator) if (fmin > fthismax) continue;
						}
						if (profileBox->frameRateMax.denominator)
						{
							if (profileInfo.frameRateMin.denominator) if (fmax < fthismin) continue;
							if (profileInfo.frameRateMax.denominator) if (fmax < fthismax) continue;
						}

						ok = TRUE;
						break;
					}

					if (!filtered || ok)
						profiles.push_back(
						profileInfo
						);
				}
			}
		}

		UINT32 Capture::MatchProfiles(
			PXCCapture::Device::StreamInfo &streamInfo,
			VideoStreamProfileList &profiles,
			UINT32 streamIndex
			)
		{
			typedef PXCCapture::VideoStream::DataDesc::StreamDesc VideoStreamDesc;

			UINT32 filled = 0;
			UINT32 desc = 0, stream = 0;
			for (; desc < VideoMapping.size(); stream = 0, desc++)
			{
				for (; stream < Sample::MaxStreams; stream++)
				{
					if (VideoMapping[desc].u32[stream] != UINT_ERROR) continue;
					if (!VideoDescs[desc]->streams[stream].format) break;

					VideoStreamDesc &streamDesc = VideoDescs[desc]->streams[stream];

					UINT32 formatMask = streamDesc.format & PXCImage::IMAGE_TYPE_MASK;
					if (streamInfo.imageType != formatMask) continue;

					UINT32 profilesCount = (UINT32) profiles.size();
					VideoStreamProfileList::iterator p = profiles.begin();
					for (; p != profiles.end(); p++)
					{
						BOOL erase = FALSE;
						if (formatMask == PXCImage::IMAGE_TYPE_DEPTH && streamDesc.format != p->imageInfo.format) erase = TRUE;
						if (streamDesc.sizeMin.width > 0 && p->imageInfo.width < streamDesc.sizeMin.width) erase = TRUE;
						if (streamDesc.sizeMin.height > 0 && p->imageInfo.height < streamDesc.sizeMin.height) erase = TRUE;
						if (streamDesc.sizeMax.width > 0 && p->imageInfo.width > streamDesc.sizeMax.width) erase = TRUE;
						if (streamDesc.sizeMax.height > 0 && p->imageInfo.height > streamDesc.sizeMax.height) erase = TRUE;

						if (erase)
						{
							profilesCount--;
							if (!filled || profilesCount != UINT_ERROR)
								p = profiles.erase(p);
							else p++;
						}
						else
						{
							p++;
						}
					}

					if (!profilesCount) continue;

					VideoMapping[desc].u32[stream] = streamIndex;
					filled++;
				}
			}

			return filled;
		}

		Capture::VideoStreamProfile *Capture::GetBestProfile(VideoStreamProfileList &profiles, UINT32 streamIndex)
		{
			Capture::VideoStreamProfile &profileInfo
				= *profiles.begin();

			if ((profileInfo.imageInfo.format & PXCImage::IMAGE_TYPE_MASK) !=
				PXCImage::IMAGE_TYPE_COLOR)
				return nullptr;

			UINT32 desc = 0, stream = 0;
			for (; desc < VideoMapping.size(); stream = 0, desc++)
			{
				for (; stream < Sample::MaxStreams; stream++)
				{
					if (VideoMapping[desc].u32[stream] != streamIndex)
						continue;

					auto p = profiles.begin();
					for (; p != profiles.end(); p++)
					{
						if (VideoDescs[desc]->streams[stream].format !=
							p->imageInfo.format) continue;

						profileInfo = *p;
						return nullptr;
					}
				}
			}

			return nullptr;
		}

		BOOL Capture::RecordProperties()
		{
			Status status;
			for (auto &cap : DeviceCaps)
				status = Device->SetProperty(
				cap.first, cap.second
				);

			for (auto &desc : VideoDescs)
			for (UINT32 devcap = 0; devcap < MaxDevCaps; devcap++)
			if (!desc->devCaps[devcap].label) break; else
			{
				auto label = desc->devCaps[devcap].label;
				Check() = Device->QueryProperty(
					(PXCCapture::Device::Property)abs(label),
					(&desc->devCaps[devcap].value)
					);
				if (Failed() && label > 0)
					return FALSE;
			}

			return TRUE;
		}

		void Capture::DeleteStreams()
		{
			for (auto &stream : VideoStreams)
				stream->Release();
			VideoStreams.clear();
		}

		Capture::Status Capture::LocateStreams()
		{

			DescFilter.group = PXCSession::IMPL_GROUP_SENSOR;
			DescFilter.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;

			UINT32 streamsRequired = CalculateStreamCount(&VideoDescs);
			for (UINT32 captureIndex = 0;; captureIndex++)
			{
				Check() = CreateBase(captureIndex, Base.ReleaseRef());

				if (Check() == PXC_STATUS_ITEM_UNAVAILABLE) break;
				else if (Failed()) continue;

				PXCCapture::DeviceInfo deviceInfo;
				for (UINT32 deviceIndex = 0;; deviceIndex++)
				{
					Check() = Base->QueryDevice(deviceIndex, &deviceInfo);
					if (Failed()) break;

					if (DeviceFilter.didx > 0) if (deviceInfo.didx != DeviceFilter.didx) continue;
					if (DeviceFilter.name[0]) if (!wcsstr(deviceInfo.name, DeviceFilter.name)) continue;
					if (DeviceFilter.did[0]) if (!wcsstr(deviceInfo.did, DeviceFilter.did)) continue;

					Check() = Base->CreateDevice(deviceIndex, Device.ReleaseRef());
					if (Failed()) continue;

					Clear();
					UINT32 streamsTotalMatched = 0;

					for (UINT32 streamIndex = 0;; streamIndex++)
					{
						PXCCapture::Device::StreamInfo streamInfo;
						Check() = Device->QueryStream(streamIndex, &streamInfo);
						if (Failed()) break;


						if (streamInfo.cuid == PXCCapture::VideoStream::CUID &&
							streamsTotalMatched < streamsRequired)
						{
							PXCSmartPtr<PXCCapture::VideoStream> videoStream;

							Check() = Device->CreateStream(
								streamIndex, PXCCapture::VideoStream::CUID,
								(void**) &videoStream);
							if (Failed()) break;

							VideoStreamProfileList profiles;
							ScanProfiles(profiles, videoStream);

							UINT32 profilesMatched = (UINT32) MatchProfiles(
								streamInfo, profiles, (UINT32) VideoStreams.size()
								);
							if (!profilesMatched) continue;

							VideoStreamProfile *bestProfile = GetBestProfile(
								profiles, (UINT32) VideoStreams.size()
								);
							//if (!bestProfile) continue;

							Check() = videoStream->SetProfile(
								&*profiles.begin()
								);
							if (Failed()) break;

							VideoStreams.push_back(videoStream.ReleasePtr());
							streamsTotalMatched += profilesMatched;
						}

						if (Succeeded() && streamsTotalMatched >= streamsRequired) break;
					}

					if (Succeeded() && streamsTotalMatched >= streamsRequired)
					if (RecordProperties())
						break;

					DeleteStreams();
					Device.ReleaseRef();
				}

				if (Succeeded())
				{
					UINT32 video;
					UINT32 desc = 0, stream = 0;
					UINT32 minSize = (UINT32) min(VideoStreams.size(), VideoMapping.size());
					for (; desc < minSize; stream = 0, desc++)
					for (; stream < Sample::MaxStreams; stream++)
					if ((video = VideoMapping[desc].u32[stream]) == UINT_ERROR ||
						!VideoStreams[VideoMapping[desc].u32[stream]]) break;
					else
					{
						VideoStreamProfile profileInfo;
						PXCCapture::VideoStream *videoStream = VideoStreams[video];
						if (Utility::Succeeded(videoStream->QueryProfile(&profileInfo)))
						{
							VideoDescs[desc]->streams[stream].sizeMin.width = profileInfo.imageInfo.width;
							VideoDescs[desc]->streams[stream].sizeMin.height = profileInfo.imageInfo.height;
							VideoDescs[desc]->streams[stream].sizeMax.width = profileInfo.imageInfo.width;
							VideoDescs[desc]->streams[stream].sizeMax.height = profileInfo.imageInfo.height;
						}
					}

					break;
				}

				Base.ReleaseRef();
			}

			return Check();
		}
	}
}