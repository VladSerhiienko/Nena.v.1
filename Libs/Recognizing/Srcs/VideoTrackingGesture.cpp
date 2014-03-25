#include "Video.h"

namespace Nena
{
	namespace Video
	{
		namespace Tracking
		{

			Gesture::Action Gesture::EnableGesture(_In_ pxcCHAR *name) { Enabled = true, Utility::CharArrayCopy(Desc.friendlyName, name); }
			Gesture::Action Gesture::EnableGesture(_In_ pxcUID iuid) { Enabled = true, Desc.iuid = iuid; }
			Gesture::Action Gesture::PauseGesture(_In_ BOOL paused) { Paused = paused; }

			Gesture::Gesture(Capture *capture) : 
				Gesture::Gesture(capture  // if the pointer is not null
					? capture->Session.IsValid() // and its a valid instance
					? capture->Session.operator->() // then return it
					: nullptr : nullptr // otherwise return null and call init later 
					)
			{
			}

			Gesture::Gesture(PXCSession *session) : 
				Utility::BasicStatus<>(),
				Base(), Desc(), Profile(),
				DescIndex(UINT_ERROR),
				Enabled(TRUE),
				Paused(FALSE)
			{
				Utility::GenericZeroMemory(&Desc);
				Utility::GenericZeroMemory(&Profile);

				Init(session);
			}

			Gesture::Action Gesture::Init(PXCSession *session)
			{
				if (session) // if valid, then create gesture base
					Check() = session->CreateImpl<PXCGesture>(
						&Desc, Base.ReleaseRef()
						);
				// otherwise set error
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Gesture::Action Gesture::Init(Capture *capture)
			{
				return Init(capture->Session);
			}

			Gesture::Action Gesture::SetProfile() 
			{
				if (Base.IsValid() && // ensure we wont have memory access violations
					// check if suitable profile was found 
					DescIndex != UINT_ERROR && // and we have some inputs for capture
					Utility::GetStreamCountFromDesc(&Profile.inputs))
						// only if everything is ok, try confirm gesture profile
						Check() = Base->SetProfile(&Profile);
				// otherwise something went wrong
				// gesture instance is invalid at this moment
				else Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Gesture::Action Gesture::SearchProfile(UINT32 option)
			{
				if (Base.IsValid()) // if gesture instance was created
					if (option == DefaultProfile) // check default profile option
						Check() = Base->QueryProfile(&Profile); // try check working profile
					else if (option == FirstSuccessfulQuery) // check search profile option
					{
						for (option = 0;; option++) // loop until good profile found
						if (Check() = Base->QueryProfile(option, &Profile), 
							Succeeded()) break;
					}
					// otherwise user wants to try a certain profile at pos
					else Check() = Base->QueryProfile(
						option, &Profile 
						);
			}

			Gesture::Action Gesture::SubmitRequirements(_Inout_ Capture *host) 
			{
				// check if we need something to submit to allocate video streams
				if (host && Utility::GetStreamCountFromDesc(&Profile.inputs))
					DescIndex = host->VideoDescs.size(), // set so we then know how to create sample from capture
					host->VideoDescs.push_back(&Profile.inputs), // submit requirements
					Check() = PXC_STATUS_NO_ERROR; // everything went ok
				else Check() = PXC_STATUS_HANDLE_INVALID, // capture was null
					DescIndex = UINT_ERROR; // index undefined
			}

			Gesture::Status Gesture::SubmitProcessTask(_Inout_ Sample *sample)
			{
				if (Base.IsValid() && !Paused && sample)
					return Check() = Base->ProcessImageAsync(
						sample->Images, &sample->SyncOut
						);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Gesture::Status Gesture::SubmitProcessTask(_Inout_ Sample *sample, _In_ UINT32 sidx)
			{
				if (Base.IsValid() && !Paused && sample && sidx < Sample::MaxStreams)
					return Check() = Base->ProcessImageAsync(
						sample->Images, &sample->SyncsInter[sidx]
						);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Gesture::Status Gesture::SubmitProcessTask(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync)
			{
				if (Base.IsValid() && !Paused && images && sync)
					return Check() = Base->ProcessImageAsync(
						images, sync
						);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Gesture::Status Gesture::SubmitProcessTaskUnsafe(_Inout_ Sample *sample)
			{
				return Base->ProcessImageAsync(
					sample->Images, &sample->SyncOut
					);
			}

			Gesture::Status Gesture::SubmitProcessTaskUnsafe(_Inout_ Sample *sample, _In_ UINT32 sidx)
			{
				return Base->ProcessImageAsync(
					sample->Images, &sample->SyncsInter[sidx]
					);
			}

			Gesture::Status Gesture::SubmitProcessTaskUnsafe(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync)
			{
				return Base->ProcessImageAsync(
					images, sync
					);
			}

			Gesture::Status Gesture::Subscribe(_In_ PXCGesture::Gesture::Handler *gesture)
			{
				if (Base.IsValid() && (Profile.sets || !gesture))
					return Check() = Base->SubscribeGesture(
						gesture ? 100 : 0, gesture
						);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			Gesture::Status Gesture::Subscribe(_In_ PXCGesture::Alert::Handler *alert)
			{
				if (Base.IsValid() && (Profile.alerts || !alert))
					return Check() = Base->SubscribeAlert(alert);
				else return Check() = PXC_STATUS_HANDLE_INVALID;
			}

			void PXCAPI Gesture::DebugOutput::OnGesture(PXCGesture::Gesture *data)
			{
#if defined(DEBUG) || defined(_DEBUG)
				static WCHAR s_msg[64] = { 0 };
				OutputDebugStringA("Gesture::DebugOutput::OnGesture -> \n");

				if (Utility::Equals(data->label,PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT))
					wsprintfW(s_msg, L"\tLABEL_NAV_SWIPE_LEFT(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT))
					wsprintfW(s_msg, L"\tLABEL_NAV_SWIPE_RIGHT(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN))
					wsprintfW(s_msg, L"\tLABEL_NAV_SWIPE_DOWN(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_UP))
					wsprintfW(s_msg, L"\tLABEL_NAV_SWIPE_UP(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_HAND_CIRCLE))
					wsprintfW(s_msg, L"\tLABEL_HAND_CIRCLE(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_HAND_WAVE))
					wsprintfW(s_msg, L"\tLABEL_HAND_WAVE(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_POSE_BIG5))
					wsprintfW(s_msg, L"\tLABEL_POSE_BIG5(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_POSE_PEACE))
					wsprintfW(s_msg, L"\tLABEL_POSE_PEACE(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN))
					wsprintfW(s_msg, L"\tLABEL_POSE_THUMB_DOWN(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if (Utility::Equals(data->label, PXCGesture::Gesture::LABEL_POSE_THUMB_UP))
					wsprintfW(s_msg, L"\tLABEL_POSE_THUMB_UP(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else wsprintfW(s_msg, L"\tLABEL_UNKNOWN(%d)\n", data->label),
					OutputDebugStringW(s_msg);
#endif
			}

			void PXCAPI Gesture::DebugOutput::OnAlert(PXCGesture::Alert *data)
			{
#if defined(DEBUG) || defined(_DEBUG)
				static WCHAR s_msg[64] = { 0 };
				OutputDebugStringA("Gesture::DebugOutput::OnAlert -> \n");

				if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_GEONODE_ACTIVE)))
					wsprintfW(s_msg, L"\tLABEL_GEONODE_ACTIVE(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_GEONODE_INACTIVE)))
					wsprintfW(s_msg, L"\tLABEL_GEONODE_INACTIVE(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_FOV_BLOCKED)))
					wsprintfW(s_msg, L"\tLABEL_FOV_BLOCKED(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_FOV_BOTTOM)))
					wsprintfW(s_msg, L"\tLABEL_FOV_BOTTOM(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_FOV_LEFT)))
					wsprintfW(s_msg, L"\tLABEL_FOV_LEFT(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_FOV_OK)))
					wsprintfW(s_msg, L"\tLABEL_FOV_OK(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_FOV_RIGHT)))
					wsprintfW(s_msg, L"\tLABEL_FOV_RIGHT(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else if ((Utility::Equals(data->label, PXCGesture::Alert::LABEL_FOV_TOP)))
					wsprintfW(s_msg, L"\tLABEL_FOV_TOP(%d)\n", data->label),
					OutputDebugStringW(s_msg);
				else wsprintfW(s_msg, L"\tLABEL_UNKNOWN(%d)\n", data->label),
					OutputDebugStringW(s_msg);
#endif
			}


		}
	}
}