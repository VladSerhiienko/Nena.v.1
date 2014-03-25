#include "VideoTrackingFacePerc.h"

namespace Nena
{
	namespace Video
	{
		namespace Perc
		{
			namespace Tracking
			{
				Face::Face(Capture *capture) :
					Face::Face(capture
						? capture->Session.IsValid()
						? capture->Session.operator->()
						: nullptr : nullptr
						)
				{
				}

				Face::Face(PXCSession *session) :
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

				Face::Action Face::Init(Capture *capture)
				{
					return Init(capture->Session);
				}

				Face::Action Face::Init(PXCSession *session)
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

				Face::Action Face::PauseFaceAnalysis(BOOL paused)
				{
					Paused = paused;
				}

				Face::Action Face::SubmitRequirements(Capture *host)
				{
					if (host &&
						Utility::GetStreamCountFromDesc(&Profile.inputs))
						DescIndex = (UINT32) host->VideoDescs.size(),
						host->VideoDescs.push_back(&Profile.inputs),
						Check() = PXC_STATUS_NO_ERROR;
					else Check() = PXC_STATUS_HANDLE_INVALID,
						DescIndex = UINT_ERROR;
				}

				Face::Action Face::SetProfile(void)
				{
					if (Base.IsValid() &&
						DescIndex != UINT_ERROR &&
						Utility::GetStreamCountFromDesc(&Profile.inputs))
						Check() = Base->SetProfile(&Profile);
					else Check() = PXC_STATUS_HANDLE_INVALID;
				}

				Face::Action Face::SetLandmarkProfile(void)
				{
					if (Base.IsValid() && Landmark &&
						LandmarkProfile.labels)
						Check() = Landmark->SetProfile(&LandmarkProfile);
					else Check() = PXC_STATUS_HANDLE_INVALID;
				}

				Face::Action Face::SetDetectionProfile(void)
				{
					if (Base.IsValid() && Detection &&
						DetectionProfile.responsiveness)
						Check() = Detection->SetProfile(&DetectionProfile);
					else Check() = PXC_STATUS_HANDLE_INVALID;
				}

				Face::Action Face::SearchProfile(UINT32 option)
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

				Face::Action Face::SearchLandmarkProfile(UINT32 option)
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

				Face::Action Face::SearchDetectionProfile(UINT32 option)
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

				Face::Status Face::SubmitProcessTask(Sample *sample)
				{
					if (Base.IsValid() && !Paused && sample)
						return Check() = Base->ProcessImageAsync(sample->Images, &sample->SyncOut);
					else return Check() = PXC_STATUS_HANDLE_INVALID;
				}

				Face::Status Face::SubmitProcessTask(Sample *sample, UINT32 sidx)
				{
					if (Base.IsValid() && !Paused && sample && sidx < Sample::MaxStreams)
						return Check() = Base->ProcessImageAsync(sample->Images, &sample->SyncsInter[sidx]);
					else return Check() = PXC_STATUS_HANDLE_INVALID;
				}

				Face::Status Face::SubmitProcessTask(PXCImage *images [], PXCScheduler::SyncPoint **sync)
				{
					if (Base.IsValid() && !Paused && images && sync)
						return Check() = Base->ProcessImageAsync(images, sync);
					else return Check() = PXC_STATUS_HANDLE_INVALID;
				}

				Face::Status Face::SubmitProcessTaskUnsafe(Sample *sample)
				{
					return Base->ProcessImageAsync(sample->Images, &sample->SyncOut);
				}

				Face::Status Face::SubmitProcessTaskUnsafe(Sample *sample, UINT32 syncIdx)
				{
					return Base->ProcessImageAsync(sample->Images, &sample->SyncsInter[syncIdx]);
				}

				Face::Status Face::SubmitProcessTaskUnsafe(PXCImage *images [], PXCScheduler::SyncPoint **sync)
				{
					return Base->ProcessImageAsync(images, sync);
				}

				UINT32 Face::GetLastFaceCount(Status *lastStatus)
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
}