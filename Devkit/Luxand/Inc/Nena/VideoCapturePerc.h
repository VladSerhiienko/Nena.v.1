#include "VideoUtilityPerc.h"

#ifndef _NENA_VIDEOCAPTUREPERC_INCLUDED__
#define _NENA_VIDEOCAPTUREPERC_INCLUDED__

namespace Nena
{
	namespace Video
	{
		namespace Perc
		{
			struct Capture;

			//! This class is used to sample and sync video streams efficiently and easy.
			//! Use Capture class to allocate samples according to required inputs.
			//! Samples are stored in Capture class instance
			struct Sample : public Utility::BasicStatus<>
			{
				friend Capture;
				friend class PXCSmartAsyncImplIxOy<Sample, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, 1>;

				static UINT32 const MaxStreams = PXCCapture::VideoStream::STREAM_LIMIT;

				typedef void Action;
				typedef HRESULT Result;
				typedef pxcStatus Status;
				typedef std::list<Sample*> List;
				typedef Status(*SampleCallback)(_Inout_ Sample *sample);
				typedef Status(*SampleCallbackEx)(_Inout_ Sample *sample, _Out_range_(1, MaxStreams) PXCImage *[], _Outref_ PXCScheduler::SyncPoint **);

				UINT32 StreamCount;
				UINT32 StreamIndices[MaxStreams];
				PXCImage *Images[MaxStreams];
				PXCCapture::VideoStream *VideoStreams[MaxStreams];
				PXCScheduler::SyncPoint *SyncOut;
				PXCScheduler::SyncPoint *SyncsInter[MaxStreams];

				inline UINT32 GetStreamCount(void) const { return StreamCount; }
				inline UINT32 const *GetStreamIndices(void) const { return StreamIndices; }

				Sample::Status SubmitReadTask(void);
				Sample::Status AwaitReadTask(void);

				Sample::Status SubmitReadTask(
					_Out_range_(0, MaxStreams) PXCImage *images[],
					_Outref_ PXCScheduler::SyncPoint **sync
					);

				Sample::Action ReleaseImages();
				Sample::Status SynchronizeSyncsInter(void);
				Sample::Status SynchronizeSyncsInter(_In_ UINT32(&order)[MaxStreams]);

			protected:

				Capture *Host;
				SampleCallback OnSubmitCallback;
				SampleCallbackEx OnSubmitCallbackEx;

				static Status OnSubmit_Choose(_Inout_ Sample *);
				static Status OnSubmit_SingleStream(_Inout_ Sample *);
				static Status OnSubmit_MultipleStream(_Inout_ Sample *);
				static Status OnSubmitEx_Choose(_Inout_ Sample *, _Out_range_(1, MaxStreams) PXCImage *[], _Outref_ PXCScheduler::SyncPoint **);
				static Status OnSubmitEx_SingleStream(_Inout_ Sample *, _Out_range_(1, MaxStreams) PXCImage *[], _Outref_ PXCScheduler::SyncPoint **);
				static Status OnSubmitEx_MultipleStream(_Inout_ Sample *, _Out_range_(1, MaxStreams) PXCImage *[], _Outref_ PXCScheduler::SyncPoint **);

				pxcStatus PXCAPI ReadSync(_In_ PXCImage *[], _In_ PXCScheduler::SyncPoint *[]);
				pxcStatus PXCAPI PassOnStatus(_In_ Status status);

				Sample(_In_ Capture *host, _In_range_(1, MaxStreams) UINT32 streamCount = MaxStreams);
				~Sample();

			};

			//! This class can be used to locate streams according to desired video stream descriptions. 
			//! It creates all the resources needed and ensures all the created resources are valid and 
			//! compatible with each other
			struct Capture : public Utility::BasicStatus<>
			{
				friend Sample;
				friend class PXCSmartAsyncImplIxOy<Capture, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, 1>;

				static UINT32 const MaxDevCaps = PXCCapture::VideoStream::DEVCAP_LIMIT;

				typedef void Action;
				typedef HRESULT Result;
				typedef pxcStatus Status;
				typedef Nena::Utilities::Neon::uint32x4_t Neon128;
				typedef std::vector<Neon128> Desc2StreamMap;
				typedef concurrency::task<void> ActionAsync;
				typedef concurrency::task<Result> ResultAsync;
				typedef concurrency::task<Status> StatusAsync;
				typedef PXCCapture::VideoStream* VideoStreamPack[Sample::MaxStreams];
				typedef PXCCapture::VideoStream::ProfileInfo VideoStreamProfile;
				typedef std::list<VideoStreamProfile> VideoStreamProfileList;
				typedef std::vector<VideoStreamPack> VideoStreamPackVector;
				typedef std::vector<PXCCapture::VideoStream*> VideoStreamVector;
				typedef PXCCapture::VideoStream::DataDesc VideoStreamDataDesc;
				typedef std::vector<VideoStreamDataDesc*> VideoStreamDataDescVector;
				typedef std::map<UINT32, PXCCapture::VideoStream::ProfileInfo> VideoProfileMap;
				typedef std::multimap<UINT32, std::pair<UINT32, PXCCapture::VideoStream::ProfileInfo>> VideoProfileMultimap;
				typedef std::map<PXCCapture::Device::Property, pxcF32> DeviceProperty2ValueMap;

				Sample::List Samples;
				Desc2StreamMap VideoMapping;
				PXCSmartPtr<PXCSession> Session;
				PXCSmartPtr<PXCScheduler> Scheduler;
				PXCSmartPtr<PXCCapture> Base;
				PXCSmartPtr<PXCCapture::Device> Device;
				VideoStreamVector VideoStreams;
				PXCSession::ImplDesc DescFilter;
				PXCCapture::DeviceInfo DeviceFilter;
				DeviceProperty2ValueMap DeviceCaps;
				VideoStreamProfileList VideoFilters;
				VideoStreamDataDescVector VideoDescs;

				static UINT32 CalculateStreamCount(_In_ VideoStreamDataDesc &desc);
				static UINT32 CalculateStreamCount(_In_ VideoStreamDataDesc *desc);
				static UINT32 CalculateStreamCount(_In_ VideoStreamDataDescVector &desc);
				static UINT32 CalculateStreamCount(_In_ VideoStreamDataDescVector *desc);

				Capture(PXCSession *session = NULL);
				~Capture(void);

				Capture::Status LocateStreams(void);
				BOOL GetSampleFromSelf(_Inout_ _Notnull_ Sample **sample);
				Sample * _Ret_notnull_ GetNewSample(_In_opt_ _In_range_(1, Sample::MaxStreams) UINT32 streamCount = Sample::MaxStreams, _Out_ UINT32 *index = NULL);
				Sample * _Ret_maybenull_ GetSampleFromDesc(_In_ UINT32 videoDescIndex = 0);

			protected:

				UINT32 MatchProfiles(_In_ PXCCapture::Device::StreamInfo &streamInfo, _In_ VideoStreamProfileList &profiles, _In_ UINT32 streamIndex);
				Action ScanProfiles(_Out_ VideoStreamProfileList &profiles, _In_ _Notnull_ PXCCapture::VideoStream *videoStream);
				Capture::VideoStreamProfile * _Ret_maybenull_ GetBestProfile(_In_ VideoStreamProfileList &profiles, _In_ UINT32 streamIndex);
				Status CreateBase(_In_ UINT32 profileIndex, _Out_ PXCCapture **capture);
				BOOL RecordProperties(void);
				Action DeleteStreams(void);
				Action DeleteSamples(void);
				Action Clear(void);

			};
		}
	}
}

#endif // !_NENA_VIDEOCAPTUREPERC_INCLUDED__
