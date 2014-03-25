#include "VideoCapture.h"


namespace Nena
{
	namespace Video2
	{

		class UtilTrace
		{
		public:
			UtilTrace(const pxcCHAR *task_name, PXCSessionService *ss) { this->ss = ss; if (ss) ss->TraceBegin(task_name); }
			~UtilTrace() { if (ss) ss->TraceEnd(); }
		protected:
			PXCSessionService *ss;
		};

		void Capture::SetFilter(PXCSession::ImplDesc &desc) { DescFilters = desc; }
		void Capture::SetFilter(PXCCapture::VideoStream::ProfileInfo *pinfo) { VideoFilters.push_back(*pinfo); }
		void Capture::SetFilter(pxcCHAR *name) { wcscpy_s<sizeof(DeviceFilters.name) / sizeof(pxcCHAR)>(DeviceFilters.name, name); }
		void Capture::SetFilter(PXCCapture::DeviceInfo *dinfo) { DeviceFilters = (*dinfo); }
		void Capture::SetFilter(PXCCapture::Device::Property p, pxcF32 v) { DevcapFilters[p] = v; }

		bool Capture::RecordProperties(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs)
		{
			for (std::map<PXCCapture::Device::Property, pxcF32>::iterator itrp = DevcapFilters.begin(); itrp != DevcapFilters.end(); itrp++)
				Device->SetProperty(itrp->first, itrp->second);

			/* The device must be able to provide all requested properties */
			for (std::vector<PXCCapture::VideoStream::DataDesc*>::iterator input = vinputs.begin(); input != vinputs.end(); input++)
			{
				for (int i = 0; i<PXCCapture::VideoStream::DEVCAP_LIMIT; i++)
				{
					if (!(*input)->devCaps[i].label) break;
					int label = (*input)->devCaps[i].label;
					pxcStatus sts = Device->QueryProperty((PXCCapture::Device::Property)(label>0 ? label : -label), &(*input)->devCaps[i].value);
					if (sts<PXC_STATUS_NO_ERROR && label>0) return false;
				}
			}
			return true;
		}

		void Capture::ScanProfiles(std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, PXCCapture::VideoStream *vstream)
		{
			for (int profile_idx = 0;; profile_idx++)
			{
				PXCCapture::VideoStream::ProfileInfo pinfo;
				pxcStatus sts = vstream->QueryProfile(profile_idx, &pinfo);
				if (sts < PXC_STATUS_NO_ERROR) break;

				if (VideoFilters.empty())
				{
					profiles.push_back(pinfo);
				}
				else
				{
					bool filtered = false, ok = false;
					for (std::list<PXCCapture::VideoStream::ProfileInfo>::iterator vs = VideoFilters.begin(); vs != VideoFilters.end(); vs++)
					{
						if ((vs->imageInfo.format&PXCImage::IMAGE_TYPE_MASK) != (pinfo.imageInfo.format&PXCImage::IMAGE_TYPE_MASK)) continue;
						filtered = true;
						if (vs->imageInfo.width && vs->imageInfo.width != pinfo.imageInfo.width) continue;
						if (vs->imageInfo.height && vs->imageInfo.height != pinfo.imageInfo.height) continue;
						if (vs->frameRateMin.denominator)
						{
							float fmin = (float) vs->frameRateMin.numerator / vs->frameRateMin.denominator;
							if (pinfo.frameRateMin.denominator)	if (fmin > (float) pinfo.frameRateMin.numerator / pinfo.frameRateMin.denominator) continue;
							if (pinfo.frameRateMax.denominator)	if (fmin > (float) pinfo.frameRateMax.numerator / pinfo.frameRateMax.denominator) continue;
						}
						if (vs->frameRateMax.denominator)
						{
							float fmax = (float) vs->frameRateMax.numerator / vs->frameRateMax.denominator;
							if (pinfo.frameRateMin.denominator)	if (fmax < (float) pinfo.frameRateMin.numerator / pinfo.frameRateMin.denominator) continue;
							if (pinfo.frameRateMax.denominator)	if (fmax < (float) pinfo.frameRateMax.numerator / pinfo.frameRateMax.denominator) continue;
						}
						ok = true;
						break;
					}
					if (!filtered || ok) profiles.push_back(pinfo);
				}
			}
		}

		int Capture::MatchProfiles(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, PXCCapture::Device::StreamInfo &sinfo, std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, int vstream)
		{
			int filled = 0;
			for (size_t i = 0; i < VideoMaps.size(); i++)
			{
				for (size_t j = 0; j < PXCCapture::VideoStream::STREAM_LIMIT; j++)
				{
					if (VideoMaps[i][j] >= 0) continue;
					PXCCapture::VideoStream::DataDesc::StreamDesc &sdesc = vinputs[i]->streams[j];
					if (!sdesc.format) break;
					if (sinfo.imageType != (sdesc.format&PXCImage::IMAGE_TYPE_MASK)) continue;

					size_t nprofiles = profiles.size();
					for (std::list<PXCCapture::VideoStream::ProfileInfo>::iterator p = profiles.begin(); p != profiles.end();)
					{
						bool erase = false;
						if ((sdesc.format&PXCImage::IMAGE_TYPE_MASK) == PXCImage::IMAGE_TYPE_DEPTH && sdesc.format != p->imageInfo.format) erase = true;
						if (sdesc.sizeMin.width > 0 && p->imageInfo.width<sdesc.sizeMin.width) erase = true;
						if (sdesc.sizeMin.height>0 && p->imageInfo.height<sdesc.sizeMin.height) erase = true;
						if (sdesc.sizeMax.width>0 && p->imageInfo.width > sdesc.sizeMax.width) erase = true;
						if (sdesc.sizeMax.height > 0 && p->imageInfo.height > sdesc.sizeMax.height) erase = true;

						if (erase)
						{
							nprofiles--;
							if (filled == 0 || nprofiles > 0) p = profiles.erase(p);
							else p++;
						}
						else
						{
							p++;
						}
					}
					if (nprofiles == 0) continue;

					VideoMaps[i][j] = vstream;
					filled++;
				}
			}
			return filled;
		}

		void Capture::ClearMaps(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs)
		{
			VideoMaps.clear();
			VideoMaps.resize(vinputs.size());
			for (size_t i = 0; i < vinputs.size(); i++)
			{
				VideoMaps[i].resize(PXCCapture::VideoStream::STREAM_LIMIT);
				for (size_t j = 0; j < PXCCapture::VideoStream::STREAM_LIMIT; j++)
					VideoMaps[i][j] = -1;
			}
		}

		void Capture::FindBestProfile(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, int vstream)
		{
			PXCCapture::VideoStream::ProfileInfo &pinfo = (*profiles.begin());
			if ((pinfo.imageInfo.format&PXCImage::IMAGE_TYPE_MASK) != PXCImage::IMAGE_TYPE_COLOR) return;
			// for color image, we try the exact match.
			for (size_t i = 0; i < VideoMaps.size(); i++)
			{
				for (size_t j = 0; j < PXCCapture::VideoStream::STREAM_LIMIT; j++)
				{
					if (VideoMaps[i][j] != vstream) continue;
					for (std::list<PXCCapture::VideoStream::ProfileInfo>::iterator p = profiles.begin(); p != profiles.end(); p++)
					{
						if (vinputs[i]->streams[j].format != p->imageInfo.format) continue;
						pinfo = (*p);
						return;
					}
				}
			}
		}

		int Capture::CalculateNumFormats(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs)
		{
			int n1 = 0;
			for (size_t i = 0; i < vinputs.size(); i++)
			for (size_t j = 0; j < PXCCapture::VideoStream::STREAM_LIMIT; j++)
			if (vinputs[i] && vinputs[i]->streams[j].format) n1++; else break;
			return n1;
		}

		pxcStatus Capture::CreateCapture(pxcU32 index, PXCCapture **capture)
		{
			PXCSession::ImplDesc desc2;
			pxcStatus sts = Session->QueryImpl(&DescFilters, index, &desc2);
			if (sts < PXC_STATUS_NO_ERROR) return PXC_STATUS_ITEM_UNAVAILABLE;
			return Session->CreateImpl<PXCCapture>(&desc2, capture);
		}

		template <class T> bool __inline Consolidate(T &v1, T &v2)
		{
			if (!v2) return true;
			if (v1 && v1 != v2) return false;
			v1 = v2;
			return true;
		}

		pxcStatus Capture::LocateStreams(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs)
		{
			UtilTrace trace(L"UtilCapture::LocateStreams(video)", SessionService);
			if (vinputs.empty()) return PXC_STATUS_ITEM_UNAVAILABLE;

			int required_formats = CalculateNumFormats(vinputs);

			pxcStatus sts;
			DescFilters.group = PXCSession::IMPL_GROUP_SENSOR;
			DescFilters.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
			for (int module_idx = 0;; module_idx++)
			{
				sts = CreateCapture(module_idx, Base.ReleaseRef());
				if (sts == PXC_STATUS_ITEM_UNAVAILABLE) break;
				if (sts < PXC_STATUS_NO_ERROR) continue;

				// Loop through all of the devices supported by the capture module to find one
				// that provides streams that match the required streams.
				PXCCapture::DeviceInfo dinfo;
				for (int device_idx = 0;; device_idx++)
				{
					sts = Base->QueryDevice(device_idx, &dinfo);
					if (sts<PXC_STATUS_NO_ERROR) break;
					if (DeviceFilters.name[0]) if (!wcsstr(dinfo.name, DeviceFilters.name)) continue;
					if (DeviceFilters.did[0]) if (!wcsstr(dinfo.did, DeviceFilters.did)) continue;
					if (DeviceFilters.didx>0) if (dinfo.didx != DeviceFilters.didx) continue;

					if (SessionService) SessionService->TraceParam(L"Locating stream(s) on device ", dinfo.name);
					sts = Base->CreateDevice(device_idx, Device.ReleaseRef());
					if (sts < PXC_STATUS_NO_ERROR) continue;

					/* Match image formats */
					ClearMaps(vinputs);

					int total_matched = 0;

					for (int stream_idx = 0;; stream_idx++)
					{
						PXCCapture::Device::StreamInfo sinfo;
						sts = Device->QueryStream(stream_idx, &sinfo);
						if (sts < PXC_STATUS_NO_ERROR) break;

						// If this stream is a video stream (audio is checked below)
						// and there are still more streams that need to be matched then
						// check to see if this stream matches what is required.
						if (sinfo.cuid == PXCCapture::VideoStream::CUID && total_matched < required_formats)
						{
							PXCSmartPtr<PXCCapture::VideoStream> vstream;
							sts = Device->CreateStream(stream_idx, PXCCapture::VideoStream::CUID, (void**) &vstream);
							if (sts < PXC_STATUS_NO_ERROR) break;

							std::list<PXCCapture::VideoStream::ProfileInfo> profiles;
							ScanProfiles(profiles, vstream);

							// Each stream has its own set of profiles. Check the streams to see how many
							// match the required profile.
							int num_matched = MatchProfiles(vinputs, sinfo, profiles, (int) VideoStreams.size());
							if (num_matched == 0) continue;

							// Once the profiles are matched find the best one and set if for the stream
							// that will be used for this capture.
							FindBestProfile(vinputs, profiles, (int) VideoStreams.size());
							sts = vstream->SetProfile(&*profiles.begin());
							if (sts < PXC_STATUS_NO_ERROR) break;

							VideoStreams.push_back(vstream.ReleasePtr());
							total_matched += num_matched;
						}

						if (sts >= PXC_STATUS_NO_ERROR && total_matched >= required_formats) break;
					}
					if (sts >= PXC_STATUS_NO_ERROR && total_matched >= required_formats)
					if (RecordProperties(vinputs)) break;

					// Unable to match all of the stream profiles for what was required so delete
					// whatever streams we had selected up to this point and release the device
					// so that the loop can check the next device.
					DeleteStreams();
					Device.ReleaseRef();
				}
				if (sts >= PXC_STATUS_NO_ERROR)
				{
					if (SessionService) SessionService->TraceParam(L"Successfully located streams on device ", dinfo.name);
					// update actual image size
					for (size_t i = 0; i < vinputs.size() && i < VideoMaps.size(); i++)
					{
						for (size_t c = 0; c < PXCCapture::VideoStream::STREAM_LIMIT; c++)
						{
							PXCCapture::VideoStream::ProfileInfo info;
							if (VideoMaps[i][c] < 0) break;
							PXCCapture::VideoStream* vstream = VideoStreams[VideoMaps[i][c]];
							if (!vstream) break;
							if (vstream->QueryProfile(&info) >= PXC_STATUS_NO_ERROR)
							{
								vinputs[i]->streams[c].sizeMin.width = info.imageInfo.width;
								vinputs[i]->streams[c].sizeMin.height = info.imageInfo.height;
								vinputs[i]->streams[c].sizeMax.width = info.imageInfo.width;
								vinputs[i]->streams[c].sizeMax.height = info.imageInfo.height;
							}
						}
					}
					break;
				}
				Base.ReleaseRef();
			}
			return sts;
		}

		pxcStatus Capture::LocateStreams(PXCCapture::VideoStream::DataDesc* vinput)
		{
			std::vector<PXCCapture::VideoStream::DataDesc*> vinputs;
			vinputs.push_back(vinput);
			return LocateStreams(vinputs);
		}

		pxcStatus Capture::ReadStreamAsync(PXCImage *images [], PXCScheduler::SyncPoint **sp)
		{
			UtilTrace trace(L"UtilCapture::ReadStreamAsync(video)", SessionService);
			pxcU32 nvstreams = (pxcU32) VideoStreams.size();
			if (VideoMaps.size() == 1)
			{   // for a single I/O, we use the VideoMaps order.
				if (nvstreams == 1)
					return VideoStreams.front()->ReadStreamAsync(images, sp);

				PXCSmartSPArray spts(PXCCapture::VideoStream::STREAM_LIMIT);
				for (pxcU32 s = 0; s < nvstreams; s++)
				{
					pxcStatus sts = VideoStreams[VideoMaps[0][s]]->ReadStreamAsync(&images[s], &spts[s]);
					if (sts < PXC_STATUS_NO_ERROR) return sts;
				}
				return PXCSmartAsyncImplIxOy<Capture, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT>::
					SubmitTask(images, nvstreams, spts.ReleasePtrs(), nvstreams, sp, this, Scheduler, &Capture::ReadStreamSync, &Capture::PassOnStatus, L"UtilCapture::ReadStreamSync");
			}
			else
			{    // for multiple I/O, we use the VideoStreams order. Each I/O is supposed to use MapStreams to get the right view.
				PXCSmartSPArray spts(PXCCapture::VideoStream::STREAM_LIMIT);
				for (pxcU32 s1 = 0; s1 < nvstreams; s1++)
				{
					pxcStatus sts = VideoStreams[s1]->ReadStreamAsync(&images[s1], &spts[s1]);
					if (sts < PXC_STATUS_NO_ERROR) return sts;
				}
				return PXCSmartAsyncImplIxOy<Capture, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT>::
					SubmitTask(images, nvstreams, spts.ReleasePtrs(), nvstreams, sp, this, Scheduler, &Capture::ReadStreamSync, &Capture::PassOnStatus, L"UtilCapture::ReadStreamSync");
			}
		}

		pxcStatus Capture::ReadStreamSync(PXCImage * [], PXCScheduler::SyncPoint *spts [])
		{
			pxcU32 nvstreams = (pxcU32) VideoStreams.size();
			if (spts[0]) spts[0]->SynchronizeEx(nvstreams, spts);
			for (pxcU32 i = 0; i < nvstreams; i++) if (spts[i]) spts[i]->Release();
			return PXC_STATUS_NO_ERROR;
		}

		Capture::Capture(PXCSession *session)
		{
			pxcStatus sts;
			if (!session) sts = PXCSession_Create(&Session);
			else Session = session;

			SessionService = Session->DynamicCast<PXCSessionService>();
			Session->CreateScheduler(&Scheduler);
			memset(&DescFilters, 0, sizeof(DescFilters));
			memset(&DeviceFilters, 0, sizeof(DeviceFilters));
		}

		Capture::~Capture(void)
		{
			//DeleteStreams();
		}

		void Capture::DeleteStreams(void)
		{
			for (std::vector<PXCCapture::VideoStream*>::iterator s = VideoStreams.begin(); s != VideoStreams.end(); s++)
				(*s)->Release();
			VideoStreams.clear();
		}

		pxcStatus Capture::MapImages(int input, PXCImage *in [], PXCImage *out [])
		{
			if (VideoMaps.size() == 1) // for a single I/O, images already reordered
			{
				int nvstreams = (int) VideoStreams.size();
				for (int s = 0; s < nvstreams; s++) out[s] = in[s];
				return PXC_STATUS_NO_ERROR;
			}
			for (size_t i = 0; i < (size_t) PXCCapture::VideoStream::STREAM_LIMIT; i++)
			{
				if (VideoMaps[(size_t) input][i] < 0) break;
				out[i] = in[VideoMaps[(size_t) input][i]];
			}
			return PXC_STATUS_NO_ERROR;
		}

		PXCImage *Capture::QueryImage(PXCImage *images [], PXCImage::ImageType type)
		{
			for (size_t i = 0; i < VideoStreams.size(); i++)
			{
				PXCImage::ImageInfo info;
				images[i]->QueryInfo(&info);
				if ((info.format&PXCImage::IMAGE_TYPE_MASK) == (type&PXCImage::IMAGE_TYPE_MASK)) return images[i];
			}
			return NULL;
		}

		PXCImage *Capture::QueryImageByFormat(PXCImage *images [], PXCImage::ColorFormat format)
		{
			for (size_t i = 0; i < VideoStreams.size(); i++)
			{
				PXCImage::ImageInfo info;
				images[i]->QueryInfo(&info);
				if (info.format == format) return images[i];
			}
			return NULL;
		}

		PXCCapture::VideoStream *Capture::QueryVideoStream(int channel, int input)
		{
			if (input >= (int) VideoMaps.size()) return 0;
			if (channel >= PXCCapture::VideoStream::STREAM_LIMIT) return 0;
			if (VideoMaps[(size_t) input][(size_t) channel] < 0) return 0;
			return VideoStreams[VideoMaps[(size_t) input][channel]];
		}

		void Capture::SetFilter(PXCImage::ImageType type, PXCSizeU32 &size, pxcU32 fps)
		{
			PXCCapture::VideoStream::ProfileInfo pinfo;
			memset(&pinfo, 0, sizeof(pinfo));
			pinfo.imageInfo.format = (PXCImage::ColorFormat)type;
			pinfo.imageInfo.width = size.width;
			pinfo.imageInfo.height = size.height;
			pinfo.frameRateMin.numerator = fps;
			pinfo.frameRateMin.denominator = fps ? 1 : 0;
			VideoFilters.push_back(pinfo);
		}
	}
}

namespace Nena
{
	namespace Video2
	{

		namespace Processing
		{
			void Gesture::EnableGesture(pxcUID iuid)
			{
				Enabled = true;
				Desc.iuid = iuid;
			}

			void Gesture::EnableGesture(pxcCHAR *name)
			{
				Enabled = true;
				wcscpy_s<sizeof(Desc.friendlyName) / sizeof(pxcCHAR)>(
					Desc.friendlyName, name
					);
			}

			void Gesture::PauseGesture(BOOL paused)
			{
				Paused = paused;
			}

			Gesture::Gesture(PXCSession *host)
			{
				Utility::GenericZeroMemory(&Desc);
				Utility::GenericZeroMemory(&Profile);

				pxcStatus sts = host->CreateImpl<PXCGesture>(
					&Desc, Base.ReleaseRef()
					);
			}

			pxcStatus Gesture::SubmitProcessTask(
				PXCImage *images [],
				PXCScheduler::SyncPoint **sync
				)
			{
				return Base->ProcessImageAsync(images, sync);
			}

			void Gesture::SetProfile(
				UINT32 p
				)
			{
				if (p == UINT_ERROR)
					for (p = 0;; p++)
					{
						pxcStatus sts = Base->QueryProfile(
							p, &Profile
							);
						if (sts >= PXC_STATUS_NO_ERROR) 
							break;
					}
				else
				{
					pxcStatus sts = Base->QueryProfile(
						p, &Profile
						);
				}

			}

			void Gesture::LocateStreams(
				std::vector<PXCCapture::VideoStream::DataDesc*> &descs
				)
			{
				descs.push_back(
					&Profile.inputs
					);
			}

			GestureAdv::GestureAdv(PXCSession *host) : Gesture(host)
			{
			}
			
			void PXCAPI GestureAdv::OnGesture(_In_ PXCGesture::Gesture *data)
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

			void PXCAPI GestureAdv::OnAlert(_In_ PXCGesture::Alert *data)
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

			pxcStatus GestureAdv::UnsubscribeGesture()
			{
				return Base->SubscribeGesture(0,0);
			}

			pxcStatus GestureAdv::UnsubscribeAlert()
			{
				return Base->SubscribeAlert(0);
			}

			pxcStatus GestureAdv::SubscribeGesture()
			{
				return Base->SubscribeGesture(100, this);
			}

			pxcStatus GestureAdv::SubscribeAlert()
			{
				return Base->SubscribeAlert(this);
			}

		}
	}
}
