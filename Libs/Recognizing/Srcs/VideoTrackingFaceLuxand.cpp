#include "Video.h"

#define _NenaVideo_Luxand_PrefDevLicRec			"App/Video/Tracking/LuxandSDK/License"
#define _NenaVideo_Luxand_PrefDevIdxRec			"App/Video/Tracking/LuxandSDK/Device/Preferred/Index"
#define _NenaVideo_Luxand_PrefDevNameRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Name"

#define _NenaVideo_Luxand_PrefDevBppMaxRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Format/BPPMax"
#define _NenaVideo_Luxand_PrefDevBppMinRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Format/BPPMin"
#define _NenaVideo_Luxand_PrefDevWMaxRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Format/WMax"
#define _NenaVideo_Luxand_PrefDevWMinRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Format/WMin"
#define _NenaVideo_Luxand_PrefDevHMaxRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Format/HMax"
#define _NenaVideo_Luxand_PrefDevHMinRec		"App/Video/Tracking/LuxandSDK/Device/Preferred/Format/HMin"

#define _NenaVideo_Luxand_TrackerTtgSxRec		"App/Video/Tracking/LuxandSDK/Tracker/Default/Target/Gender"
#define _NenaVideo_Luxand_TrackerTtgNmRec		"App/Video/Tracking/LuxandSDK/Tracker/Default/Target/Name"

namespace Nena
{
	namespace Video
	{
		namespace Tracking
		{

			FaceLuxand::Tracker::PerFrameData::PerFrameData(
				Tracker *host, UINT32 size, UINT32 namesLen
				) : Host(host), Count(size), 
					Bytes(size * sizeof INT64),
					NamesBytes(namesLen * sizeof CHAR)
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::Tracker::PerFrameData::PerFrameData()\n");
#endif //NDEBUG

				Ids = new INT64[size]; assert(Ids);
				Names = new CHAR[namesLen]; assert(Names);

				Utility::GenericZeroMemory(&Pos);
				Utility::GenericZeroMemory(Ids, (size_t) Bytes);
				Utility::GenericZeroMemory(Names, (size_t) NamesBytes);
			}

			FaceLuxand::Tracker::PerFrameData::~PerFrameData()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::Tracker::PerFrameData::~PerFrameData()\n");
#endif //NDEBUG

				if (Names) delete[] Names, Names = NULL;
				if (Ids) delete[] Ids, Ids = NULL;
			}

			FaceLuxand::Status FaceLuxand::Tracker::PerFrameData::StoreFacePosById(_In_ INT64 id)
			{
				return Check() = FSDK_GetTrackerFacePosition(Host->Handle, Host->Device->Handle, id, &Pos);
			}

			FaceLuxand::Status FaceLuxand::Tracker::PerFrameData::StoreFacePosByIdIndex(_In_ INT64 idx)
			{
				return Check() = FSDK_GetTrackerFacePosition(Host->Handle, Host->Device->Handle, Ids[idx], &Pos);
			}

			FaceLuxand::Status FaceLuxand::Tracker::PerFrameData::StoreFaceNamesById(_In_ INT64 id)
			{
				return Check() = FSDK_GetTrackerFacePosition(Host->Handle, Host->Device->Handle, id, &Pos);
			}

			FaceLuxand::Status FaceLuxand::Tracker::PerFrameData::StoreFaceNamesByIdIndex(_In_ INT64 idx)
			{
				return Check() = FSDK_GetAllNames(Host->Handle, Ids[idx], Names, NamesBytes);
				//return Check() = FSDK_GetName(Host->Handle, Host->Device->Handle, Names, NamesBytes);
			}

			FaceLuxand::Status FaceLuxand::Tracker::PerFrameData::SetFaceNameById(_In_ INT64 id)
			{
				return Check() = FSDK_GetTrackerFacePosition(Host->Handle, Host->Device->Handle, id, &Pos);
			}

			FaceLuxand::Status FaceLuxand::Tracker::PerFrameData::SetFaceNameByIdIndex(_In_ INT64 idx)
			{
				Check() = FSDK_LockID(Host->Handle, Ids[idx]);
				if (Succeeded()) FSDK_SetName(Host->Handle, Ids[idx], Names);
				return Check() = FSDK_UnlockID(Host->Handle, Ids[idx]);
			}

			FaceLuxand::Tracker::Tracker(FaceLuxand *faceLuxand, HTracker handle) :
				Utility::BasicStatus<INT32>(), Host(faceLuxand), Handle(handle)
			{
			}

			FaceLuxand::Tracker::~Tracker()
			{
				ClearPerFrameData();
				Check() = FSDK_FreeTracker(Handle);
			}



			FaceLuxand::Tracker::PerFrameData *FaceLuxand::Tracker::QueryPerFrameData(
				UINT32 size, UINT32 namesLen
				)
			{
				PerFrame.push_back(new PerFrameData(this, size));
				return PerFrame.front();
			}

			FaceLuxand::Status FaceLuxand::Tracker::ClearPerFrameData()
			{
				for (auto &data : PerFrame)
					delete data,
					data = NULL;

				PerFrame.clear();
				return FSDKE_OK;
			}

			FaceLuxand::Status FaceLuxand::Tracker::SetParameter(
				String8 const & param, String8 const & value
				)
			{
				return Check() = FSDK_SetTrackerParameter(
					Handle, param.data(), value.data()
					);
			}

			FaceLuxand::Status FaceLuxand::Tracker::SetMultipleParameters(
				FaceLuxand::Tracker::Target target, INT32 *error
				)
			{
				String8 params;

				App::Vars *vars = App::Vars::GetForCurrentThread();
				if (!vars->GetRecordsCount()) if (Utility::Failed(vars->Read()))
					return FSDKE_FILE_NOT_FOUND;

				switch (target)
				{
				case Nena::Video::Tracking::FaceLuxand::Tracker::kGender:
					if (vars->HasRecord(_NenaVideo_Luxand_TrackerTtgSxRec))
						params = vars->GetRecord<String8>(_NenaVideo_Luxand_TrackerTtgSxRec);
					else return FSDKE_PARAMETER_NOT_FOUND; break;
				case Nena::Video::Tracking::FaceLuxand::Tracker::kName:
					if (vars->HasRecord(_NenaVideo_Luxand_TrackerTtgNmRec))
						params = vars->GetRecord<String8>(_NenaVideo_Luxand_TrackerTtgNmRec);
					else return FSDKE_PARAMETER_NOT_FOUND; break;
				default: return FSDKE_PARAMETER_NOT_FOUND;
				}

				return SetMultipleParameters(params, error);
			}

			FaceLuxand::Status FaceLuxand::Tracker::SetMultipleParameters(
				String8 const & params, INT32 *error
				)
			{
				return Check() = FSDK_SetTrackerMultipleParameters(
					Handle, params.c_str(), error
					);
			}

			FaceLuxand::Status FaceLuxand::Tracker::GrabFrame(void)
			{
				return Check() = FSDK_GrabFrame(Device->Handle, &ImageHandle);
			}

			FaceLuxand::Status FaceLuxand::Tracker::FeedFrame(_In_ PerFrameData *perFrame)
			{
				return Check() = FSDK_FeedFrame(
					Handle, Device->Index, ImageHandle,
					&perFrame->FacesFound, perFrame->Ids,
					perFrame->Bytes
					);
			}

			FaceLuxand::Status FaceLuxand::Tracker::FreeFrame(_In_ PerFrameData *perFrame)
			{
				return Check() = FSDK_FreeImage(ImageHandle);
			}

		}
	}
}

namespace Nena
{
	namespace Video
	{
		namespace Tracking
		{
			FaceLuxand::Camera::~Camera()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::Camera::~Camera()\n");
#endif //NDEBUG

				ClearFormats();
				Close();
			}

			FaceLuxand::Camera::Camera(FaceLuxand *faceLuxand, INT32 index, WCHAR *name) :
				Name(name), Index(index), Host(faceLuxand), Handle(InvalidHandle),
				Formats(NULL), Format(NULL), FormatCount(0)
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::Camera::Camera()\n");
#endif //NDEBUG
				QueryFormats();
			}

			FaceLuxand::Status FaceLuxand::Camera::SetFormat(INT32 formatIndex)
			{
				if (Check() = FSDK_SetVideoFormat(Name.c_str(), Formats[formatIndex]), Succeeded()) 
					Format = &Formats[formatIndex];
				return Check();
			}

			FaceLuxand::Status FaceLuxand::Camera::SetFormat(VideoFormat &format)
			{
				if (Check() = FSDK_SetVideoFormat(Name.c_str(), format), Succeeded())
					Format = &format;
				return Check();
			}

			FaceLuxand::Status FaceLuxand::Camera::SetFormat(VideoFormat *format)
			{
				if (!format) return SetFormat(0);
				if (Check() = FSDK_SetVideoFormat(Name.c_str(), (*format)), Succeeded())
					Format = format;
				return Check();
			}

			FaceLuxand::Status FaceLuxand::Camera::QueryFormats()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::Camera::QueryFormats()\n");
#endif //NDEBUG

				if (FormatCount) return FSDKE_OK; else
				{
					INT32 videoFormatCount = 0;
					Check() = FSDK_GetVideoFormatList(Name.c_str(), &Formats, &videoFormatCount);
					if (Succeeded())
					{
						FormatCount = (UINT32) videoFormatCount;
#if DEBUG //DEBUGGING
						for (INT32 fmt = 0; fmt < FormatCount; fmt++)
						{
							Platform::DebugTrace(
								"\tWidth=%4d Height=%4d BPP=%3d\n", 
								Formats[fmt].Width, 
								Formats[fmt].Height, 
								Formats[fmt].BPP
								);
						}
#endif //NDEBUG
					}
#if DEBUG //DEBUGGING
					else
					{

						Platform::DebugTrace("[ERR][FSDK] Failed to get video format list\n");
					}
#endif //NDEBUG
					return Check();
				}
			}

			FaceLuxand::Status FaceLuxand::Camera::ClearFormats()
			{
				if (!FormatCount) return FSDKE_OK; else
				{
					Format = NULL;
					if (Formats) return Check() = FSDK_FreeVideoFormatList(Formats);
					return FSDKE_OK;
				}
			}

			FaceLuxand::Status FaceLuxand::Camera::Open()
			{
				if (Handle != InvalidHandle) return FSDKE_OK;
				return Check() = FSDK_OpenVideoCamera(Name.c_str(), &Handle);
			}

			FaceLuxand::Status FaceLuxand::Camera::Close()
			{
				if (Handle == InvalidHandle) return FSDKE_OK;
				Check() = FSDK_CloseVideoCamera(Handle);
				Handle = InvalidHandle;
				return Check();
			}

		}
	}
}

namespace Nena
{
	namespace Video
	{
		namespace Tracking
		{
			UINT32 FaceLuxand::sm_instanceCount = 0u;
			BOOL FaceLuxand::sm_activated = FALSE;
			BOOL FaceLuxand::sm_initialized = FALSE;
			BOOL FaceLuxand::sm_capturingInitialized = FALSE;

			FaceLuxand::FaceLuxand() : Utility::BasicStatus<INT32>(),
				m_cameraNames(NULL), m_cameraCount(0),
				Activated(FALSE), Device(NULL)
			{
				sm_instanceCount++;
				assert(sm_instanceCount == 1);

				Check() = Activate(), 
					Activated = Succeeded();
				Check() = Initialize();
				Check() = InitializeCapturing();
			}

			FaceLuxand::~FaceLuxand()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::~FaceLuxand()\n");
#endif //NDEBUG

				sm_instanceCount--;

				ClearTrackers();
				ClearDevices();

				FinalizeCapturing();
				Finalize();
			}

			FaceLuxand::Tracker *FaceLuxand::QueryNewTracker()
			{
				HTracker handle;
				Status errorOnCreate = FSDK_CreateTracker(&handle);
				if (Utility::Succeeded(errorOnCreate))
					Trackers.push_back(new Tracker(this, handle)),
					Trackers.back()->Index = Trackers.size() - 1,
					Trackers.back()->Device = Device;
				else return nullptr;
				return Trackers.back();
			}

			FaceLuxand::Status FaceLuxand::DeleteTracker(Tracker *tracker)
			{
				if (tracker->Host == this && (*&Trackers[tracker->Index]) == tracker)
					Trackers.erase(Trackers.begin() + tracker->Index), 
					delete tracker, tracker = NULL;
				else return FSDKE_INVALID_ARGUMENT;
				return FSDKE_OK;
			}

			FaceLuxand::Status FaceLuxand::ClearTrackers()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::ClearTrackers()\n");
#endif //NDEBUG

				// through all trackers
				for (auto &tracker : Trackers)
					delete tracker, // erase tracker
					tracker = NULL; // avoid access violations
				Trackers.clear(); // clear collection
				return FSDKE_OK; // ok
			}

			FaceLuxand::Status FaceLuxand::QueryDevices()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::QueryDevices()\n");
#endif //NDEBUG

				if (ClearDevices(), Check() = FSDK_GetCameraList(&m_cameraNames, &m_cameraCount), Succeeded())
				{
					for (INT32 cameraIndex = 0; cameraIndex < m_cameraCount; cameraIndex++)
					{
#if DEBUG //DEBUGGING
						Platform::DebugTrace("\tCamera#%d: ", cameraIndex);
						OutputDebugStringW(m_cameraNames[cameraIndex]);
						OutputDebugStringA("\n");
#endif //NDEBUG

						Devices.push_back(new Camera(this, cameraIndex, m_cameraNames[cameraIndex]));
					}

					Device = Devices.front();

#if DEBUG //DEBUGGING
					OutputDebugStringA("\tCurrent cam: ");
					OutputDebugStringW(Device->Name.c_str());
					OutputDebugStringA("\n");
#endif //NDEBUG
					return Check();
				}
				else
				{
					Platform::DebugTrace("\t[ERR][FSDK] Failed to get camera list\n");
					return FSDKE_FAILED;
				}
			}

			FaceLuxand::Status FaceLuxand::ClearDevices()
			{
#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::ClearDevices()\n");
#endif //NDEBUG
				if (!m_cameraNames || !m_cameraCount)
					return FSDKE_OK;

				// through all devices
				for (auto &device : Devices) 
					delete device, // erase camera device
					device = NULL; // avoid access violations
				Check() = FSDK_FreeCameraList(m_cameraNames, m_cameraCount);
				Devices.clear(); // clear collection
				return Check();
			}

			FaceLuxand::Status FaceLuxand::SetDevice(UINT32 deviceIndex)
			{
				if (!Devices.empty())
				{
					if (deviceIndex == UINT_ERROR)
					{
						UINT32 preferredDeviceIndex = deviceIndex;

						auto vars = App::Vars::GetForCurrentThread();
						if (!vars->GetRecordsCount()) if (Utility::Failed(vars->Read()))
							return FSDKE_FILE_NOT_FOUND;

						if (vars->HasRecord(_NenaVideo_Luxand_PrefDevIdxRec))
							preferredDeviceIndex = vars->GetRecord<UINT32>(_NenaVideo_Luxand_PrefDevIdxRec);
						if (preferredDeviceIndex < Devices.size()) deviceIndex = preferredDeviceIndex;
					}

					if (deviceIndex < Devices.size()) Device = *&Devices[deviceIndex]; 
					if (!Device) Device = *&Devices.front();
					return Device ? FSDKE_OK : FSDKE_FAILED;
				}
				else return FSDKE_FAILED;
			}

			FaceLuxand::Status FaceLuxand::SetDeviceByName(String16 deviceName)
			{
				if (Devices.size())
				{
					if (deviceName.empty())
					{
						String16 preferredName;

						auto vars = App::Vars::GetForCurrentThread();
						if (!vars->GetRecordsCount()) if (Utility::Failed(vars->Read()))
							return FSDKE_FILE_NOT_FOUND;

						if (vars->HasRecord(_NenaVideo_Luxand_PrefDevNameRec))
							preferredName = vars->GetRecord<String16>(_NenaVideo_Luxand_PrefDevNameRec);
						if (!preferredName.empty()) deviceName = preferredName;
					}

					for (auto &device : Devices) if (device->Name == deviceName) Device = *&device;
					if (!Device) Device = *&Devices.front();
					return Device ? FSDKE_OK : FSDKE_FAILED;
				}
				else return FSDKE_FAILED;
			}

			FaceLuxand::Status FaceLuxand::SetDeviceByName(String8 deviceName)
			{
				return SetDeviceByName(Nena::Converter::String8To16(deviceName));
			}

			FaceLuxand::Status FaceLuxand::SetDeviceByFormat(VideoFormat *min, VideoFormat *max)
			{
				typedef std::multimap<Camera *, VideoFormat*> String2FormatMm;

#if DEBUG //DEBUGGING
				Platform::DebugTrace("FaceLuxand::SetDeviceByFormat()\n");
#endif //NDEBUG

				String16 preferredName;
				FSDK_VideoFormatInfo minimal; Utility::GenericZeroMemory(&minimal);
				FSDK_VideoFormatInfo maximal; Utility::GenericZeroMemory(&maximal);
				maximal.Height = GetSystemMetrics(SM_CYSCREEN);
				maximal.Width = GetSystemMetrics(SM_CXSCREEN);
				maximal.BPP = INT_MAX;

				auto vars = App::Vars::GetForCurrentThread();
				if (!vars->GetRecordsCount()) if (Utility::Failed(vars->Read()))
					return FSDKE_FILE_NOT_FOUND;

				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevNameRec)) preferredName = vars->GetRecord<String16>(_NenaVideo_Luxand_PrefDevNameRec);
				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevBppMaxRec)) maximal.BPP = vars->GetRecord<INT32>(_NenaVideo_Luxand_PrefDevBppMaxRec);
				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevBppMinRec)) minimal.BPP = vars->GetRecord<INT32>(_NenaVideo_Luxand_PrefDevBppMinRec);
				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevWMaxRec)) maximal.Width = vars->GetRecord<INT32>(_NenaVideo_Luxand_PrefDevWMaxRec);
				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevWMinRec)) minimal.Width = vars->GetRecord<INT32>(_NenaVideo_Luxand_PrefDevWMinRec);
				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevHMaxRec)) maximal.Height = vars->GetRecord<INT32>(_NenaVideo_Luxand_PrefDevHMaxRec);
				if (vars->HasRecord(_NenaVideo_Luxand_PrefDevHMinRec)) minimal.Height = vars->GetRecord<INT32>(_NenaVideo_Luxand_PrefDevHMinRec);

				if (!min) min = &minimal;
				if (!max) max = &maximal;

#if DEBUG //DEBUGGING
				Platform::DebugTrace("\tPreferred configuration: \n");
				if (preferredName.size()) { OutputDebugStringA("\t> Preferred camera: "); OutputDebugStringW(preferredName.c_str()); OutputDebugStringA("\n"); }
				Platform::DebugTrace("\t> Preferred width: from %d to %d\n", minimal.Width, maximal.Width);
				Platform::DebugTrace("\t> Preferred height: from %d to %d\n", minimal.Height, maximal.Height);
				Platform::DebugTrace("\t> Preferred bpp: from %d to %d\n", minimal.BPP, maximal.BPP);
#endif //NDEBUG

				BOOL preferredDeviceFound = FALSE;
				for (auto &dvc : Devices)
					if (dvc->Name == preferredName)
					{
						preferredDeviceFound = TRUE,
							Device = dvc;

						break;
					}

#if DEBUG //DEBUGGING
					Platform::DebugTrace("\t! Preferred camera found: %s\n", (preferredDeviceFound ? "Yes" : "No"));
#endif //NDEBUG

				BOOL formatSet = FALSE;
				String2FormatMm matched;

				for (auto &dvc : Devices) for (UINT32 fmt = 0; fmt < dvc->FormatCount; fmt++)
				if (dvc->Formats[fmt].Height >= minimal.Height && dvc->Formats[fmt].Height <= maximal.Height &&
					dvc->Formats[fmt].Width >= minimal.Width && dvc->Formats[fmt].Width <= maximal.Width &&
					dvc->Formats[fmt].BPP >= minimal.BPP && dvc->Formats[fmt].BPP <= maximal.BPP)
					matched.insert(std::make_pair(dvc, &dvc->Formats[fmt]));

#if DEBUG //DEBUGGING
				Platform::DebugTrace("\t! Preferred formats found: %d\n", matched.size());
#endif //NDEBUG

				if (matched.size())
				{
					if (preferredDeviceFound)
					{
						auto dvcMatched = matched.equal_range(Device);
						for (auto &dvcpair = dvcMatched.first; dvcpair != dvcMatched.second; dvcpair++)
							if (Utility::Failed(dvcpair->first->SetFormat(dvcpair->second))) continue; 
							else { formatSet = TRUE; break; }
					}
					else
					{
						for (auto &dvcpair : matched) if (dvcpair.first->FormatCount)
							if (Utility::Failed(dvcpair.first->SetFormat(dvcpair.second))) continue;
							else { formatSet = TRUE; break; }
					}
				}

				if (Devices.size() && !formatSet)
				{
					Platform::DebugTrace("\t! Trying being brute...\n");
					for (auto &dvc : Devices)
						for (UINT32 fmt = 0; fmt < dvc->FormatCount; fmt++)
							if (Utility::Failed(dvc->SetFormat(fmt))) continue;
							else { formatSet = TRUE; break; }
				}

				if (formatSet)
				{
#if DEBUG //DEBUGGING
					Platform::DebugTrace("\tDevice and video format are set:\n");
					Platform::DebugTrace("\t\t! Current camera: "); OutputDebugStringW(Device->Name.c_str()); OutputDebugStringA("\n");
					Platform::DebugTrace("\t\t! Current format: w=%d h=%d bpp=%d\n", Device->Format->Width, Device->Format->Height, Device->Format->BPP);
#endif //NDEBUG
					return FSDKE_OK;
				}
				else
				{
					Platform::DebugTrace("\tFailed to get device and video format\n");
					return FSDKE_FAILED;
				}
			}

			FaceLuxand::Status FaceLuxand::Activate(String8 license)
			{
				if (sm_activated)
					return FSDKE_OK;

				if (!license.size())
				{
					auto vars = App::Vars::GetForCurrentThread();
					if (!vars->GetRecordsCount()) 
						if (Utility::Failed(vars->Read()))
							return FSDKE_FILE_NOT_FOUND;
					if (vars->HasRecord(_NenaVideo_Luxand_PrefDevLicRec))
						license = vars->GetRecord<>(_NenaVideo_Luxand_PrefDevLicRec);
					else return FSDKE_NOT_ACTIVATED;
				}

				Status sts = FSDK_ActivateLibrary(license.c_str());
				sm_activated = Utility::Succeeded(sts);
				return sts;
			}

			FaceLuxand::Status FaceLuxand::Initialize(String8 dataFiles)
			{
				if (sm_initialized)
					return FSDKE_OK;
				Status status;
				sm_initialized = Utility::Succeeded(
					status = FSDK_Initialize((LPSTR) dataFiles.data())
					);
				return status;
			}

			FaceLuxand::Status FaceLuxand::InitializeCapturing()
			{
				if (sm_capturingInitialized)
					return FSDKE_OK;

				Status status;
				sm_capturingInitialized = Utility::Succeeded(
					status = FSDK_InitializeCapturing()
					);
				return status;
			}

			FaceLuxand::Status FaceLuxand::FinalizeCapturing()
			{
				if (!sm_capturingInitialized)
					return FSDKE_OK;

				Status status;
				sm_capturingInitialized = !Utility::Succeeded(
					status = FSDK_FinalizeCapturing()
					);
				return status;
			}

			FaceLuxand::Status FaceLuxand::Finalize()
			{
				if (!sm_initialized)
					return FSDKE_OK;

				Status status;
				sm_initialized = !Utility::Succeeded(
					status = FSDK_Finalize()
					);
				return status;
			}

		}
	}
}