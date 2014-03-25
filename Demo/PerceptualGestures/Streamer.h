
#include "Precompiled.h"

#ifndef __NENA_PERCEPTUAL_VIDEOSTREAMS_INCLUDED__
#define __NENA_PERCEPTUAL_VIDEOSTREAMS_INCLUDED__

namespace Nena
{
	namespace Video
	{
		struct Capture;
		struct Sampler;

		struct Streamer
		{
			friend Capture;
			friend Sampler;

			typedef std::multimap<pxcU32, PXCCapture::VideoStream::ProfileInfo> ProfileMultimap;
			typedef std::map<pxcU32, PXCCapture::VideoStream::ProfileInfo> ProfileMap;

			static pxcU32 const MaxStreamsPerInstance = PXCCapture::VideoStream::STREAM_LIMIT;
			static pxcU32 GetFormatsCount(_In_ PXCCapture::VideoStream::DataDesc &desc);
			inline pxcI32 Size() { return static_cast<pxcI32>(m_actualStreamCount); }
			inline pxcU32 SizeU() { return m_actualStreamCount; }

			Capture *Host;

			PXCSmartArray<PXCCapture::VideoStream> Streams; // actual video streamer
			PXCCapture::VideoStream::DataDesc Desc; // required video streams to capture
			ProfileMultimap ScannedProfiles; // all video profiles that met requirements
			ProfileMap MatchedProfiles; // matched video stream profile

			Streamer(_In_ pxcU32 streamCount, _In_ Capture *capture = nullptr);
			Streamer(_In_ Capture *capture = nullptr);
			//~Streamer();
			//inline ~Streamer() { Close(); }

			HRESULT Init();
			void Close();

			void SetDescription(_In_ PXCCapture::VideoStream::DataDesc *desc);
			void SetDescription(_In_ PXCCapture::VideoStream::DataDesc &desc);
			pxcU32 GetFormatsCount();
			pxcBool AreProfilesSet();

		protected:

			void SetProfiles(_In_ PXCSmartPtr<PXCCapture::VideoStream> &videoStream);
			void ScanProfiles(_In_ PXCSmartPtr<PXCCapture::VideoStream> &videoStream);

			pxcBool m_isVideoStreamInputSet[MaxStreamsPerInstance];
			UINT32 m_actualStreamCount;

		};

		struct Sampler
		{
			typedef void Action; 
			typedef HRESULT Status;
			typedef concurrency::task<Action> ActionAsync;
			typedef concurrency::task<HRESULT> PXCStatusAsync, StatusAsync;

			friend class PXCSmartAsyncImplIxOy<Sampler, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, 
					PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT>;

			struct Sample
			{
				friend Sampler;
				friend Streamer;
				typedef pxcStatus Status; 


				Sample::Status *LastErrors;
				PXCImage::ImageData *Data;
				//PXCImage::ImageInfo *Info;
				Sampler *State;
				UINT32 Size;

				//void DeleteInfo();
				void DeleteData();
				void Delete();

			private:

				void Init(
					Sampler *sampler, 
					UINT32 size
					);

				Sample();
				~Sample();
			};

			typedef std::list<Sample *> SampleList;

			Streamer *Host; // actual streams
			PXCSmartArray<PXCImage> Images; // read samples
			PXCSmartSPArray Syncs; // synchronization points for async reads
			PXCSmartSP Sync; // synchronization point for sync reads

			Sampler(_In_ Streamer *host); // unsafe ctor
			Sampler(_In_ Streamer &host); // safe ctor
			void Close();

			Sample *AllocNewSample();

			Status SubmitReadTask(
				_Out_ PXCImage *images[],
				_Out_ PXCScheduler::SyncPoint **sync
				); //


			Status SubmitReadTask(); //
			Status AwaitReadTask(); //
			StatusAsync SubmitReadTaskAsync(); //
			StatusAsync AwaitReadTaskAsync(); //

			Status QueryImagesInfo(_In_ Sample *sample);//
			StatusAsync QueryImagesInfoAsync(_In_ Sample *sample); //

			Action AcquireReadAccess(_In_ Sample *sample, _In_ UINT32 image);//
			Action ReleaseAccess(_In_ Sample *sample, _In_ UINT32 image);//
			Action AcquireReadAccess(_In_ Sample *sample);//
			Action ReleaseAccess(_In_ Sample *sample);//

			ActionAsync AcquireReadAccessAsync(_In_ Sample *sample, _In_ UINT32 image);//
			ActionAsync ReleaseAccessAsync(_In_ Sample *sample, _In_ UINT32 image); //
			ActionAsync AcquireReadAccessAsync(_In_ Sample *sample); //
			ActionAsync ReleaseAccessAsync(_In_ Sample *sample); //

		protected:

			inline pxcStatus PXCAPI ReadSync(_In_ PXCImage *images[], _In_ PXCScheduler::SyncPoint *spts[]);
			inline pxcStatus PXCAPI PassOnStatus(_In_ pxcStatus status);

			SampleList m_allocatedSamples;

		};



	}
}

#endif //!__NENA_PERCEPTUAL_VIDEOSTREAM_INCLUDED__