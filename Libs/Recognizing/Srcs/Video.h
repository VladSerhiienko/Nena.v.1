#pragma once

#include "Precompiled.h"

namespace Nena
{
	namespace Video
	{
		// Utility classes to reduce trivial code in sources
		struct Utility
		{
			/// Types

			typedef void Action;
			typedef HRESULT Result;
			typedef pxcStatus Status;

			/// Functions (inline & static)

			template <typename _Ty>  inline static BOOL Failed(_Ty result);
			template <typename _Ty>  inline static BOOL Succeeded(_Ty result);
			template <> inline static BOOL Failed<INT32>(INT32 result) { return result < 0; }
			template <> inline static BOOL Failed<pxcStatus>(pxcStatus result) { return result < 0; }
			template <> inline static BOOL Failed<HRESULT>(HRESULT result) { return FAILED(result); }
			template <> inline static BOOL Succeeded<INT32>(INT32 result) { return result == 0; }
			template <> inline static BOOL Succeeded<pxcStatus>(pxcStatus result) { return result == 0; }
			template <> inline static BOOL Succeeded<HRESULT>(HRESULT result) { return SUCCEEDED(result); }
			template <typename _Ty>  static void GenericZeroMemory(_Ty *memory, size_t size = sizeof _Ty) { if (memory) ZeroMemory(memory, size); }
			template <typename _TyChar, UINT32 _NuChar> static UINT32 CharArraySize(_TyChar(&string)[_NuChar]) { return _NuChar; }
			template <typename _TyChar, UINT32 _NuChar> static errno_t CharArrayCopy(_TyChar(&to)[_NuChar], _TyChar *from) { return wcscpy_s<_NuChar>(to, from); }
			template <typename _TyFlagSrc, typename _TyFlagChk> static BOOL Checked(_TyFlagSrc src, _TyFlagChk chk) { return ((UINT32) src & (UINT32) chk) == (UINT32) chk; }
			template <typename _TyFlagSrc, typename _TyFlagChk> static BOOL Equals(_TyFlagSrc src, _TyFlagChk chk) { return (UINT32) src == (UINT32) chk; }

			static UINT32 GetStreamCountFromDesc(PXCCapture::VideoStream::DataDesc &desc);
			static UINT32 GetStreamCountFromDesc(PXCCapture::VideoStream::DataDesc *desc);

			template <typename _TyStatus = Status> struct BasicStatus
			{
				/// Data

				_TyStatus LastStatus;

				/// Functions (inline & static)

				inline BOOL IsValid(void) { return Succeeded(); }
				inline _TyStatus &Check(void) { return LastStatus; }
				inline _TyStatus const &Check(void) const { return LastStatus; }
				inline BOOL Failed(void) { return Utility::Failed(LastStatus); }
				inline BOOL Succeeded(void) { return Utility::Succeeded(LastStatus); }

				/// Functions (members)

				BasicStatus(_TyStatus init = (_TyStatus) 0) : LastStatus(init) {}

			};

		};

		struct Capture;

		//! This class is used to sample and sync video streams efficiently and easy.
		//! Use Capture class to allocate samples according to required inputs.
		//! Samples are stored in Capture class instance
		struct Sample : public Utility::BasicStatus<>
		{
			/// Friends

			friend Capture;
			friend class PXCSmartAsyncImplIxOy<Sample, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, 1>;

			/// Const

			static UINT32 const MaxStreams = PXCCapture::VideoStream::STREAM_LIMIT;

			/// Types

			typedef void Action;
			typedef HRESULT Result;
			typedef pxcStatus Status;
			typedef std::list<Sample*> List;
			typedef Status(*SampleCallback)(_Inout_ Sample *sample);
			typedef Status(*SampleCallbackEx)(_Inout_ Sample *sample, _Out_range_(1, MaxStreams) PXCImage * [], _Outref_ PXCScheduler::SyncPoint **);

			/// Data

			UINT32 StreamCount;
			UINT32 StreamIndices[MaxStreams];
			PXCImage *Images[MaxStreams];
			PXCCapture::VideoStream *VideoStreams[MaxStreams];
			PXCScheduler::SyncPoint *SyncOut;
			PXCScheduler::SyncPoint *SyncsInter[MaxStreams];

			/// Functions (inline & static)

			inline UINT32 GetStreamCount(void) const { return StreamCount; }
			inline UINT32 const *GetStreamIndices(void) const { return StreamIndices; }

			Sample::Status SubmitReadTask(void);
			Sample::Status AwaitReadTask(void);

			Sample::Status SubmitReadTask(
				_Out_range_(0, MaxStreams) PXCImage *images [],
				_Outref_ PXCScheduler::SyncPoint **sync
				);

			Sample::Action ReleaseImages();
			Sample::Status SynchronizeSyncsInter(void);
			Sample::Status SynchronizeSyncsInter(_In_ UINT32(&order)[MaxStreams]);

		protected:

			/// Data

			Capture *Host;
			SampleCallback OnSubmitCallback;
			SampleCallbackEx OnSubmitCallbackEx;

			/// Functions (inline & static)

			static Status OnSubmit_Choose(_Inout_ Sample *);
			static Status OnSubmit_SingleStream(_Inout_ Sample *);
			static Status OnSubmit_MultipleStream(_Inout_ Sample *);
			static Status OnSubmitEx_Choose(_Inout_ Sample *, _Out_range_(1, MaxStreams) PXCImage * [], _Outref_ PXCScheduler::SyncPoint **);
			static Status OnSubmitEx_SingleStream(_Inout_ Sample *, _Out_range_(1, MaxStreams) PXCImage * [], _Outref_ PXCScheduler::SyncPoint **);
			static Status OnSubmitEx_MultipleStream(_Inout_ Sample *, _Out_range_(1, MaxStreams) PXCImage * [], _Outref_ PXCScheduler::SyncPoint **);

			pxcStatus PXCAPI ReadSync(_In_ PXCImage * [], _In_ PXCScheduler::SyncPoint * []);
			pxcStatus PXCAPI PassOnStatus(_In_ Status status);

			/// Functions (members)

			Sample(_In_ Capture *host, _In_range_(1, MaxStreams) UINT32 streamCount = MaxStreams);
			~Sample();

		};

		//! This class can be used to locate streams according to desired video stream descriptions. 
		//! It creates all the resources needed and ensures all the created resources are valid and 
		//! compatible with each other
		struct Capture : public Utility::BasicStatus<>
		{
			/// Friends

			friend Sample;
			friend class PXCSmartAsyncImplIxOy<Capture, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT, PXCScheduler::SyncPoint, 1>;

			/// Const

			static UINT32 const MaxDevCaps = PXCCapture::VideoStream::DEVCAP_LIMIT;

			/// Types

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

			/// Data

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

			/// Functions (inline & static)

			static UINT32 CalculateStreamCount(_In_ VideoStreamDataDesc &desc);
			static UINT32 CalculateStreamCount(_In_ VideoStreamDataDesc *desc);
			static UINT32 CalculateStreamCount(_In_ VideoStreamDataDescVector &desc);
			static UINT32 CalculateStreamCount(_In_ VideoStreamDataDescVector *desc);

			/// Functions (members)

			Capture(PXCSession *session = nullptr);
			~Capture(void);

			Capture::Status LocateStreams(void);
			BOOL GetSampleFromSelf(_Notnull_ Sample **sample);
			Sample * _Ret_notnull_ GetNewSample(_In_opt_ _In_range_(1, Sample::MaxStreams) UINT32 streamCount = Sample::MaxStreams, _Out_ UINT32 *index = NULL);
			Sample * _Ret_maybenull_ GetSampleFromDesc(_In_ UINT32 videoDescIndex = 0);

		protected:

			/// Functions (members)

			UINT32 MatchProfiles(_In_ PXCCapture::Device::StreamInfo &streamInfo, _In_ VideoStreamProfileList &profiles, _In_ UINT32 streamIndex);
			Action ScanProfiles(_Out_ VideoStreamProfileList &profiles, _In_ _Notnull_ PXCCapture::VideoStream *videoStream);
			Capture::VideoStreamProfile * _Ret_maybenull_ GetBestProfile(_In_ VideoStreamProfileList &profiles, _In_ UINT32 streamIndex);
			Status CreateBase(_In_ UINT32 profileIndex, _Out_ PXCCapture **capture);
			BOOL RecordProperties(void);
			Action DeleteStreams(void);
			Action DeleteSamples(void);
			Action Clear(void);

		};

		namespace Tracking
		{
			struct Gesture : public Utility::BasicStatus<>
			{
				/// Const

				static UINT32 const DefaultProfile = UINT_ERROR;
				static UINT32 const FirstSuccessfulQuery = UINT_ERROR - 1;

				/// Types

				typedef void Action;
				typedef HRESULT Result;
				typedef pxcStatus Status;

				typedef struct DebugOutput :
					protected PXCGesture::Gesture::Handler,
					protected PXCGesture::Alert::Handler
				{
					typedef PXCGesture::Gesture::Handler *HGesture;
					typedef PXCGesture::Alert::Handler *HAlert;

					inline HAlert GetAlertHandler() { return this; }
					inline HGesture GetGestureHandler() { return this; }
					virtual Action PXCAPI OnAlert(_In_ PXCGesture::Alert *) override;
					virtual Action PXCAPI OnGesture(_In_ PXCGesture::Gesture *) override;
				};

				/// Data (members)

				BOOL Paused;
				BOOL Enabled;
				UINT32 DescIndex;
				PXCSession::ImplDesc Desc;
				PXCGesture::ProfileInfo Profile;
				PXCSmartPtr<PXCGesture> Base;

				/// Functions (members)

				Gesture(_In_ Capture *capture);
				Gesture(_In_ PXCSession *session);

				Action Init(_In_ Capture *capture);
				Action Init(_In_ PXCSession *session);
				Action PauseGesture(_In_ BOOL paused);
				Action EnableGesture(_In_ pxcUID iuid = 0);
				Action EnableGesture(_In_ pxcCHAR *name);
				Action SubmitRequirements(_Inout_ Capture *host);

				Action SetProfile(void);
				Action SearchProfile(_In_opt_ UINT32 option = DefaultProfile);

				Status Subscribe(_In_ PXCGesture::Gesture::Handler *gesture = nullptr);
				Status Subscribe(_In_ PXCGesture::Alert::Handler *alert = nullptr);
				Status SubmitProcessTask(_Inout_ Sample *sample);
				Status SubmitProcessTask(_In_ PXCImage *images [], _Outref_ PXCScheduler::SyncPoint **sync);
				Status SubmitProcessTask(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);
				Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample);
				Status SubmitProcessTaskUnsafe(_In_ PXCImage *images [], _Outref_ PXCScheduler::SyncPoint **sync);
				Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);

			};

			struct FacePxc : public Utility::BasicStatus<>
			{
				/// Const

				static UINT32 const DefaultProfile = UINT_ERROR;
				static UINT32 const FirstSuccessfulQuery = UINT_ERROR - 1;

				/// Types

				typedef void Action;
				typedef HRESULT Result;
				typedef pxcStatus Status;

				/// Data

				PXCSmartPtr<PXCFaceAnalysis> Base;

				PXCFaceAnalysis::Landmark *Landmark;
				PXCFaceAnalysis::Detection *Detection;

				BOOL Paused;
				BOOL Enabled;
				UINT32 DescIndex;
				PXCSession::ImplDesc Desc;
				PXCFaceAnalysis::ProfileInfo Profile;
				PXCFaceAnalysis::Landmark::ProfileInfo LandmarkProfile;
				PXCFaceAnalysis::Detection::ProfileInfo DetectionProfile;

				/// Functions (members)

				FacePxc(_In_ Capture *capture);
				FacePxc(_In_ PXCSession *session);

				Action Init(_In_ Capture *capture);
				Action Init(_In_ PXCSession *session);
				Action PauseFaceAnalysis(_In_ BOOL paused);
				Action SubmitRequirements(_Inout_ Capture *host);

				Action SetProfile(void);
				Action SetLandmarkProfile(void);
				Action SetDetectionProfile(void);
				Action SearchProfile(_In_opt_ UINT32 option = FirstSuccessfulQuery);
				Action SearchLandmarkProfile(_In_opt_ UINT32 option = FirstSuccessfulQuery);
				Action SearchDetectionProfile(_In_opt_ UINT32 option = FirstSuccessfulQuery);

				Status SubmitProcessTask(_Inout_ Sample *sample);
				Status SubmitProcessTask(_In_ PXCImage *images [], _Outref_ PXCScheduler::SyncPoint **sync);
				Status SubmitProcessTask(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);

				Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample);
				Status SubmitProcessTaskUnsafe(_In_ PXCImage *images [], _Outref_ PXCScheduler::SyncPoint **sync);
				Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);

				UINT32 GetLastFaceCount(_Inout_opt_ Status *lastStatus = nullptr);

			};

			struct FaceLuxand : public Utility::BasicStatus<INT32>
			{
				static INT32 const InvalidHandle = -1;
				static UINT32 const DoNothingOption = 0;
				static UINT32 const TryActivateOption = DoNothingOption + 1;
				static UINT32 const TryActivateAndInitializeOption = TryActivateOption + 1;
				static UINT32 const TryActivateAndInitializeCaptringOption = TryActivateAndInitializeOption + 1;

				struct Camera;
				struct Tracker;
				typedef void Action;
				typedef INT32 Status;
				typedef HRESULT Result;
				typedef std::string String8;
				typedef std::wstring String16;
				typedef String8::traits_type String8Traits;
				typedef String16::traits_type String16Traits;
				typedef std::vector<String16> String16Vector;
				typedef FSDK_VideoFormatInfo VideoFormat;
				typedef std::vector<VideoFormat> VideoFormatVector;
				typedef INT64 FaceId;
				typedef std::vector<INT64> FaceIdVector;
				typedef HImage ImageHandle;

				struct Tracker : public Utility::BasicStatus<INT32>
				{
					friend FaceLuxand;
					typedef std::vector<Tracker *> Vector;
					typedef enum Target { kGender = 1, kName = 2 };

					struct PerFrameData : public Utility::BasicStatus<INT32>
					{
						static UINT32 const DefaultSize = 256;
						static UINT32 const DefaultNamesLength = 1024;
						typedef TFacePosition FacePos;
						typedef std::vector<PerFrameData *> Vector;

						INT64 *Ids;
						INT64 Count;
						INT64 Bytes;

						FacePos Pos;
						CHAR *Names;
						INT64 NamesBytes;
						INT64 FacesFound;

						Tracker *Host;

						PerFrameData(
							_In_ _Notnull_ Tracker *host,
							_In_opt_ UINT32 size = DefaultSize,
							_In_opt_ UINT32 namesLen = DefaultNamesLength
							);
						~PerFrameData(void);

						Status SetFaceNameById(_In_ INT64);
						Status StoreFacePosById(_In_ INT64);
						Status StoreFaceNamesById(_In_ INT64);

						Status SetFaceNameByIdIndex(_In_ INT64);
						Status StoreFacePosByIdIndex(_In_ INT64);
						Status StoreFaceNamesByIdIndex(_In_ INT64);

					};

					HTracker Handle;
					HImage ImageHandle;

					Camera *Device;
					FaceLuxand *Host;

					String8 StorageFile;
					PerFrameData::Vector PerFrame;

					Tracker(
						_In_ _Notnull_ FaceLuxand *lib, 
						_In_ _Valid_ HTracker handle
						);
					~Tracker(void);

					Status GrabFrame(void);
					Status FeedFrame(_In_ PerFrameData *);
					Status FreeFrame(_In_ PerFrameData *);

					PerFrameData *QueryPerFrameData(
						_In_opt_ UINT32 size = PerFrameData::DefaultSize,
						_In_opt_ UINT32 namesLen = PerFrameData::DefaultNamesLength
						);

					Status ClearPerFrameData(void);

					Status SetParameter(
						_In_z_ String8 const & param,
						_In_z_ String8 const & value
						);

					Status SetMultipleParameters(
						_In_ Target target = kName,
						_In_ INT32 *error = NULL
						);

					Status SetMultipleParameters(
						_In_z_ String8 const & params,
						_In_ INT32 *error = NULL
						);

				private:

					UINT32 Index;

				};

				struct Camera : public Utility::BasicStatus<INT32>
				{
					friend FaceLuxand;
					typedef std::vector<Camera *> Vector;

					INT32 Index;
					INT32 Handle;
					String16 Name;
					FaceLuxand *Host;
					VideoFormat *Format;
					VideoFormat *Formats;
					UINT32 FormatCount;
					BOOL IsFormatSet;

					Camera(
						_In_ _Notnull_ FaceLuxand *lib,
						_In_ INT32 index,
						_In_ _Notnull_ WCHAR *name
						);

					~Camera(void);

					Status Open(void);
					Status Close(void);
					Status QueryFormats(void);
					Status ClearFormats(void);
					Status SetFormat(_In_ INT32 formatIndex);
					Status SetFormat(_In_ VideoFormat &format);
					Status SetFormat(_In_ VideoFormat *format);

				};

				BOOL Activated;
				Camera *Device;
				Camera::Vector Devices;
				Tracker::Vector Trackers;

				FaceLuxand(void);
				~FaceLuxand(void);

				Status QueryDevices(void);
				Status ClearDevices(void);

				Status SetDevice(_In_opt_ UINT32 deviceIndex = UINT_ERROR);
				Status SetDeviceByName(_In_opt_z_ String8 deviceName);
				Status SetDeviceByName(_In_opt_z_ String16 deviceName = L"");
				Status SetDeviceByFormat(_In_opt_ VideoFormat *min = NULL, _In_opt_ VideoFormat *max = NULL);

				Tracker *QueryNewTracker(void);
				Status DeleteTracker(_In_ Tracker *tracker);
				Status ClearTrackers(void);

				static Status Activate(_In_opt_z_ String8 license = "");
				static Status Initialize(_In_opt_z_ String8 dataFiles = "");
				static Status InitializeCapturing(void);
				static Status FinalizeCapturing(void);
				static Status Finalize(void);

			private:

				INT32 m_cameraCount;
				WCHAR **m_cameraNames;

				static BOOL sm_activated;
				static BOOL sm_initialized;
				static BOOL sm_capturingInitialized;

				static UINT32 sm_instanceCount;

			};

			struct Object : public Utility::BasicStatus<>
			{
				/// Const

				static UINT32 const QueryDefaultWorking = UINT_ERROR;
				static UINT32 const ReadFromFileConfiguration = UINT_ERROR - 1;

				/// Types

				typedef void Action;
				typedef HRESULT Result;
				typedef pxcStatus Status;
				typedef std::wstring String;

				/// Data

				BOOL Paused;
				BOOL Enabled;
				UINT32 DescIndex;
				String TrackerConfig;
				String ProfileConfig;
				PXCSession::ImplDesc Desc;
				PXCDCVTracker::ProfileInfo Profile;
				PXCSmartPtr<PXCDCVTracker> Tracker;

				/// Functions (members)

				Object(_In_ Capture *);
				Object(_In_ PXCSession *);

				Action Init(_In_ Capture *);
				Action Init(_In_ PXCSession *);
				Action PauseTracking(_In_ BOOL paused);
				Action SubmitRequirements(_Inout_ Capture *host);

				Action SetProfile(_In_ Sample *sample);
				Action SearchProfile(_In_opt_ UINT32 option = QueryDefaultWorking);

				Status SubmitProcessTask(_Inout_ Sample *sample);
				Status SubmitProcessTask(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync);
				Status SubmitProcessTask(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);
				Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample);
				Status SubmitProcessTaskUnsafe(_In_ PXCImage *images[], _Outref_ PXCScheduler::SyncPoint **sync);
				Status SubmitProcessTaskUnsafe(_Inout_ Sample *sample, _In_range_(0, Sample::MaxStreams) UINT32 syncIdx);
			};
		}
	}
}