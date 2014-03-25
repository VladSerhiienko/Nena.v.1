
#include "Precompiled.h"

#ifndef __NENA_PERCEPTUAL_VIDEOSTREAM_INCLUDED__
#define __NENA_PERCEPTUAL_VIDEOSTREAM_INCLUDED__

namespace Nena
{
	namespace Video
	{
		struct Capture;
		typedef std::list<PXCCapture::VideoStream::ProfileInfo> VideoStreamProfileList;

		template <pxcU32 _StreamCount> struct StreamPack
		{
			static const pxcU32 Count = _StreamCount;
			static_assert(Count <= PXCCapture::VideoStream::STREAM_LIMIT, "Stream count exceeds the limit");

			Capture *Host;
			PXCSmartArray<PXCCapture::VideoStream> Array; // actual video streamer

			PXCCapture::VideoStream::DataDesc Desc; // required video streams to capture
			VideoStreamProfileList ScannedProfiles[Count]; // all video profiles that met requirements
			PXCCapture::VideoStream::ProfileInfo Profiles[Count]; // matched video stream profile

			inline StreamPack(Capture *capture = nullptr);
			inline void SetProfiles(PXCSmartPtr<PXCCapture::VideoStream> &videoStream);
			inline void ScanProfiles(PXCSmartPtr<PXCCapture::VideoStream> &videoStream);
			inline pxcBool AreProfilesSet();
			inline pxcU32 GetFormatsCount();
			inline HRESULT Init();
			inline void Close();
			inline void Frame();

		protected:

			pxcBool m_isVideoStreamInputSet[Count];

		};

		template <pxcU32 _StreamCount> struct StreamPackSampler
		{
			typedef StreamPack<_StreamCount> StreamPackHost;
			typedef StreamPackSampler<_StreamCount> ThisStreamPackSample;
			friend class PXCSmartAsyncImplIxOy<ThisStreamPackSample, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, 1>;
			
			StreamPackHost *Host; // actual streams
			PXCSmartArray<PXCImage> Images; // read samples
			PXCSmartSPArray Syncs; // synchronization points for async reads
			PXCSmartSP Sync; // synchronization point for sync reads

			inline StreamPackSampler(StreamPackHost *host);
			inline pxcStatus ReadAsync();
			inline void Close();

		protected:

			inline pxcStatus PXCAPI ReadSync(PXCImage *images [], PXCScheduler::SyncPoint *spts []);
			inline pxcStatus PXCAPI PassOnStatus(pxcStatus sts);

		};

#include "StreamPack.inl"

	}
}

#endif // !__NENA_PERCEPTUAL_VIDEOSTREAM_INCLUDED__
