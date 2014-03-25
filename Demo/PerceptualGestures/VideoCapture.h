
#pragma once
#include "Precompiled.h"

namespace Nena
{
	namespace Video2
	{
		struct Utility
		{
			/// Functions (inline & static)

			template <typename _Ty>  inline static BOOL Failed(_Ty result);
			template <typename _Ty>  inline static BOOL Succeeded(_Ty result);
			template <> inline static BOOL Failed<HRESULT>(HRESULT result) { return FAILED(result); }
			template <> inline static BOOL Failed<pxcStatus>(pxcStatus result) { return FAILED(result); }
			template <> inline static BOOL Succeeded<HRESULT>(HRESULT result) { return SUCCEEDED(result); }
			template <> inline static BOOL Succeeded<pxcStatus>(pxcStatus result) { return SUCCEEDED(result); }
			template <typename _Ty>  static void GenericZeroMemory(_Ty *memory, size_t size = sizeof _Ty) { if (memory) ZeroMemory(memory, size); }

			static pxcU32 GetStreamCountFromDesc(PXCCapture::VideoStream::DataDesc &desc);

		};

		struct Capture
		{
			PXCSession *Session;
			PXCSessionService *SessionService;
			PXCSmartPtr<PXCScheduler> Scheduler;
			PXCSmartPtr<PXCCapture> Base;
			PXCSmartPtr<PXCCapture::Device> Device;
			std::vector<std::vector<INT32>> VideoMaps;
			std::vector<PXCCapture::VideoStream*>VideoStreams;

			PXCSession::ImplDesc DescFilters;
			PXCCapture::DeviceInfo DeviceFilters;
			std::list<PXCCapture::VideoStream::ProfileInfo> VideoFilters;
			std::map<PXCCapture::Device::Property, pxcF32> DevcapFilters;

			Capture(PXCSession *session = nullptr);
			~Capture(void);
			inline void Release(void) { delete this; }

			void SetFilter(pxcCHAR *name);
			void SetFilter(PXCSession::ImplDesc &desc);
			void SetFilter(PXCCapture::DeviceInfo *dinfo);
			void SetFilter(PXCCapture::Device::Property p, pxcF32 v);
			void SetFilter(PXCCapture::VideoStream::ProfileInfo *pinfo);
			void SetFilter(PXCImage::ImageType type, PXCSizeU32 &size, pxcU32 fps = 0);

			pxcStatus LocateStreams(PXCCapture::VideoStream::DataDesc *inputs);
			pxcStatus LocateStreams(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);

			PXCCapture::Device *QueryDevice(void) { return Device; }
			PXCCapture::VideoStream *QueryVideoStream(int channel, int view = 0);
			PXCCapture *QueryCapture(void) { return Base; }
			PXCImage *QueryImage(PXCImage *images [], PXCImage::ImageType type);
			PXCImage *QueryImageByFormat(PXCImage *images [], PXCImage::ColorFormat format);
			pxcStatus MapImages(int view, PXCImage *images_in [], PXCImage *images_out []);

			pxcStatus ReadStreamAsync(PXCImage *images [], PXCScheduler::SyncPoint **sp);


			pxcStatus PXCAPI CreateCapture(pxcU32 index, PXCCapture **capture);
			pxcStatus PXCAPI ReadStreamSync(PXCImage *images [], PXCScheduler::SyncPoint *spts []);
			pxcStatus PXCAPI PassOnStatus(pxcStatus sts) { return sts; }

			int  MatchProfiles(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, PXCCapture::Device::StreamInfo &sinfo, std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, int vstream);
			void ScanProfiles(std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, PXCCapture::VideoStream *vstream);
			bool RecordProperties(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
			void FindBestProfile(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, int vstream);
			void DeleteStreams(void);
			int  CalculateNumFormats(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
			void ClearMaps(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
		};

		namespace Processing
		{
			struct Gesture
			{
				Gesture(PXCSession *session);

				void SetProfile(_In_opt_ UINT32 index = UINT_ERROR);
				void LocateStreams(_Inout_ std::vector<PXCCapture::VideoStream::DataDesc*> &descs);

				void EnableGesture(_In_ pxcUID iuid = 0);
				void EnableGesture(_In_ pxcCHAR *name);
				void PauseGesture(_In_ BOOL paused);

				pxcStatus SubmitProcessTask(
					_In_ PXCImage *images [], 
					_Outref_ PXCScheduler::SyncPoint **sync
					);

				PXCSmartPtr<PXCGesture> Base;
				PXCGesture::ProfileInfo Profile;
				PXCSession::ImplDesc Desc;
				INT32 StreamIndex;
				BOOL Enabled;
				BOOL Paused;

			};

			struct GestureAdv :
				public Gesture,
				protected PXCGesture::Gesture::Handler,
				protected PXCGesture::Alert::Handler
			{
				GestureAdv(PXCSession *session);

				virtual void PXCAPI OnGesture(_In_ PXCGesture::Gesture *data);
				virtual void PXCAPI OnAlert(_In_ PXCGesture::Alert *data);

				pxcStatus UnsubscribeGesture();
				pxcStatus UnsubscribeAlert();
				pxcStatus SubscribeGesture();
				pxcStatus SubscribeAlert();

			};

		}
	}
}

