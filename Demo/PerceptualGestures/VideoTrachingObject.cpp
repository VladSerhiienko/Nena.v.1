#include "Video.h"

#define _NenaVideo_DFusion_DefCfgFolderRec		"App/Video/Tracking/DFusion/TotalImmersion/Tracker/Default/Config/Folder"
#define _NenaVideo_DFusion_DefCfgFileRec		"App/Video/Tracking/DFusion/TotalImmersion/Tracker/Default/Config/File"

namespace Nena
{
	namespace Video
	{
		namespace Tracking
		{

			Object::Object(Capture *capture) : 
				Object::Object(capture  // if the pointer is not null
				? capture->Session.IsValid() // and its a valid instance
				? capture->Session.operator->() // then return it
				: nullptr : nullptr // otherwise return null and call init later 
				)
			{ 
			}

			Object::Object(PXCSession *session) : 
				TrackerConfig(),
				ProfileConfig(),
				Paused(FALSE)
			{
				Utility::GenericZeroMemory(&Desc);
				Utility::GenericZeroMemory(&Profile);

				Init(session);
			}

			Object::Action Object::Init(Capture *capture) 
			{
				return Init(capture->Session);
			}

			Object::Action Object::Init(PXCSession *session) 
			{
				if (session) Check() = session->CreateImpl(
					&Desc, PXCDCVTracker::CUID, 
					(void **) Tracker.ReleaseRef()
					);
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Object::Action Object::PauseTracking(BOOL paused) 
			{
				Paused = paused;
			}

			Object::Action Object::SubmitRequirements(Capture *host) 
			{
				if (host && Utility::GetStreamCountFromDesc(&Profile.inputs))
					DescIndex = (UINT32) host->VideoDescs.size(),
					host->VideoDescs.push_back(&Profile.inputs),
					Check() = PXC_STATUS_NO_ERROR;
				else Check() = PXC_STATUS_HANDLE_INVALID,
					DescIndex = UINT_ERROR;
			}

			Object::Action Object::SetProfile(Sample *sample)
			{
				if (sample && Tracker.IsValid() && DescIndex != UINT_ERROR &&
					Utility::GetStreamCountFromDesc(&Profile.inputs))
				{
					if (TrackerConfig.empty())
					{
						String folder, file;

						auto vars = App::Vars::GetForCurrentThread();
						if (!vars->GetRecordsCount()) if (Utility::Failed(vars->Read())) return;

						if (vars->HasRecord(_NenaVideo_DFusion_DefCfgFolderRec)) 
							folder = vars->GetRecord<String>(_NenaVideo_DFusion_DefCfgFolderRec);
						if (vars->HasRecord(_NenaVideo_DFusion_DefCfgFileRec)) 
							file = vars->GetRecord<String>(_NenaVideo_DFusion_DefCfgFileRec);

						TrackerConfig = folder + file;
					}

					UINT32 streamsRequired = Utility::GetStreamCountFromDesc(Profile.inputs);
					if (streamsRequired != sample->StreamCount)
					{
						Check() = PXC_STATUS_HANDLE_INVALID;
						return;
					}

					PXCCapture::VideoStream::ProfileInfo streamProfile;
					sample->VideoStreams[0]->QueryProfile(&streamProfile);
					Profile.width = streamProfile.imageInfo.width;
					Profile.height = streamProfile.imageInfo.height;

					Check() = Tracker->SetProfile(TrackerConfig.c_str(), &Profile);
				}
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Object::Action Object::SearchProfile(UINT32 option)
			{
				if (Tracker.IsValid())
					if (option == QueryDefaultWorking)
						Check() = Tracker->QueryProfile(&Profile);
					else if (option == ReadFromFileConfiguration && ProfileConfig.size())
						Check() = Tracker->QueryProfile(ProfileConfig.c_str(), &Profile);
					else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Object::Status Object::SubmitProcessTask(Sample *sample)
			{
				if (Tracker.IsValid() && !Paused && sample)
					return Check() = Tracker->ProcessImageAsync(sample->Images, &sample->SyncOut);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Object::Status Object::SubmitProcessTask(PXCImage *images[], PXCScheduler::SyncPoint **sync)
			{
				if (Tracker.IsValid() && !Paused && images && sync)
					return Check() = Tracker->ProcessImageAsync(images, sync);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Object::Status Object::SubmitProcessTask(Sample *sample, UINT32 sidx)
			{
				if (Tracker.IsValid() && !Paused && sample && sidx < Sample::MaxStreams)
					return Check() = Tracker->ProcessImageAsync(sample->Images, &sample->SyncsInter[sidx]);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Object::Status Object::SubmitProcessTaskUnsafe(Sample *sample)
			{
				return Tracker->ProcessImageAsync(sample->Images, &sample->SyncOut);
			}

			Object::Status Object::SubmitProcessTaskUnsafe(PXCImage *images[], PXCScheduler::SyncPoint **sync)
			{
				return Tracker->ProcessImageAsync(images, sync);
			}

			Object::Status Object::SubmitProcessTaskUnsafe(Sample *sample, UINT32 syncIdx)
			{
				return Tracker->ProcessImageAsync(sample->Images, &sample->SyncsInter[syncIdx]);
			}

		}
	}
}