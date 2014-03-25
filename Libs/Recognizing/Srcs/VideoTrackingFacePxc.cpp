#include "Video.h"



namespace Nena
{
	namespace Video
	{
		namespace Tracking
		{
			FacePxc::FacePxc(Capture *capture) :
				FacePxc::FacePxc(capture
				? capture->Session.IsValid() 
				? capture->Session.operator->() 
				: nullptr : nullptr
				)
			{
			}

			FacePxc::FacePxc(PXCSession *session) :
				Utility::BasicStatus<>(),
				Base(), Desc(), Profile(),
				DescIndex(UINT_ERROR),
				DetectionProfile(),
				LandmarkProfile(),
				Enabled(TRUE),
				Paused(FALSE)
			{
					Utility::GenericZeroMemory(&Desc);
					Utility::GenericZeroMemory(&Profile);
					Utility::GenericZeroMemory(&LandmarkProfile);
					Utility::GenericZeroMemory(&DetectionProfile);

					Init(session);
			}

			FacePxc::Action FacePxc::Init(Capture *capture)
			{
				return Init(capture->Session);
			}

			FacePxc::Action FacePxc::Init(PXCSession *session)
			{
				if (session)
					Check() = session->CreateImpl<PXCFaceAnalysis>(
					&Desc, Base.ReleaseRef()
					);
				else Check() = PXC_STATUS_HANDLE_INVALID;

				if (Succeeded())
					Detection = Base->DynamicCast<PXCFaceAnalysis::Detection>(),
					Landmark = Base->DynamicCast<PXCFaceAnalysis::Landmark>();
			}

			FacePxc::Action FacePxc::PauseFaceAnalysis(BOOL paused)
			{
				Paused = paused;
			}

			FacePxc::Action FacePxc::SubmitRequirements(Capture *host)
			{
				if (host && 
					Utility::GetStreamCountFromDesc(&Profile.inputs))
					DescIndex = host->VideoDescs.size(),
					host->VideoDescs.push_back(&Profile.inputs), 
					Check() = PXC_STATUS_NO_ERROR;
				else Check() = PXC_STATUS_HANDLE_INVALID, 
					DescIndex = UINT_ERROR;
			}

			FacePxc::Action FacePxc::SetProfile(void)
			{
				if (Base.IsValid() && 
					DescIndex != UINT_ERROR && 
					Utility::GetStreamCountFromDesc(&Profile.inputs))
					Check() = Base->SetProfile(&Profile);
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			FacePxc::Action FacePxc::SetLandmarkProfile(void)
			{
				if (Base.IsValid() && Landmark &&
					LandmarkProfile.labels)
					Check() = Landmark->SetProfile(&LandmarkProfile);
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			FacePxc::Action FacePxc::SetDetectionProfile(void)
			{
				if (Base.IsValid() && Detection && 
					DetectionProfile.responsiveness)
					Check() = Detection->SetProfile(&DetectionProfile);
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			FacePxc::Action FacePxc::SearchProfile(UINT32 option)
			{
				if (Base.IsValid()) 
				if (option == DefaultProfile) 
					Check() = Base->QueryProfile(&Profile);
				else if (option == FirstSuccessfulQuery)
				{
					for (option = 0;; option++)
					if (Check() = Base->QueryProfile(option, &Profile),
						Succeeded()) break;
				}
				else Check() = Base->QueryProfile(option, &Profile);
			}

			FacePxc::Action FacePxc::SearchLandmarkProfile(UINT32 option)
			{
				if (Base.IsValid() && Landmark)
				if (option == DefaultProfile)
					Check() = Landmark->QueryProfile(&LandmarkProfile);
				else if (option == FirstSuccessfulQuery)
				{
					for (option = 0;; option++)
					if (Check() = Landmark->QueryProfile(option, &LandmarkProfile),
						Succeeded()) break;
				}
				else Check() = Landmark->QueryProfile(option, &LandmarkProfile);
			}

			FacePxc::Action FacePxc::SearchDetectionProfile(UINT32 option)
			{
				if (Base.IsValid() && Detection)
				if (option == DefaultProfile)
					Check() = Detection->QueryProfile(&DetectionProfile);
				else if (option == FirstSuccessfulQuery)
				{
					for (option = 0;; option++)
					if (Check() = Detection->QueryProfile(option, &DetectionProfile),
						Succeeded()) break;
				}
				else Check() = Detection->QueryProfile(option, &DetectionProfile);
			}

			FacePxc::Status FacePxc::SubmitProcessTask(Sample *sample)
			{
				if (Base.IsValid() && !Paused && sample)
					return Check() = Base->ProcessImageAsync(sample->Images, &sample->SyncOut);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			FacePxc::Status FacePxc::SubmitProcessTask(Sample *sample, UINT32 sidx)
			{
				if (Base.IsValid() && !Paused && sample && sidx < Sample::MaxStreams)
					return Check() = Base->ProcessImageAsync(sample->Images, &sample->SyncsInter[sidx]);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			FacePxc::Status FacePxc::SubmitProcessTask(PXCImage *images[], PXCScheduler::SyncPoint **sync)
			{
				if (Base.IsValid() && !Paused && images && sync)
					return Check() = Base->ProcessImageAsync(images, sync);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			FacePxc::Status FacePxc::SubmitProcessTaskUnsafe(Sample *sample)
			{
				return Base->ProcessImageAsync(sample->Images, &sample->SyncOut);
			}

			FacePxc::Status FacePxc::SubmitProcessTaskUnsafe(Sample *sample, UINT32 syncIdx)
			{
				return Base->ProcessImageAsync(sample->Images, &sample->SyncsInter[syncIdx]);
			}

			FacePxc::Status FacePxc::SubmitProcessTaskUnsafe(PXCImage *images[], PXCScheduler::SyncPoint **sync)
			{
				return Base->ProcessImageAsync(images, sync);
			}

			UINT32 FacePxc::GetLastFaceCount(Status *lastStatus)
			{
				UINT32 faceIndex; INT32 faceId;
				Status onQueryFaceStatus = PXC_STATUS_NO_ERROR;
				for (faceIndex = 0; !onQueryFaceStatus && Base.IsValid(); faceIndex++)
					if (Utility::Failed(Base->QueryFace(faceIndex, &faceId))) break;
				if (lastStatus) (*lastStatus) = onQueryFaceStatus;
				return faceIndex;
			}

		}
	}
}