#include <LuxandFaceSDK.h>
#include "VideoUtilityPerc.h"

#ifndef __NENA_VIDEOFACELXND_INCLUDED__
#define __NENA_VIDEOFACELXND_INCLUDED__

namespace Nena
{
	namespace Video
	{
		namespace Luxand
		{
			namespace Tracking
			{
				struct Face : public Utility::BasicStatus<INT32>
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
						friend Face;
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
						Face *Host;

						String8 StorageFile;
						PerFrameData::Vector PerFrame;

						Tracker(
							_In_ _Notnull_ Face *lib,
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
						friend Face;
						typedef std::vector<Camera *> Vector;

						INT32 Index;
						INT32 Handle;
						String16 Name;
						Face *Host;
						VideoFormat *Format;
						VideoFormat *Formats;
						UINT32 FormatCount;
						BOOL IsFormatSet;

						Camera(
							_In_ _Notnull_ Face *lib,
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

					Face(void);
					~Face(void);

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

			}
		}
	}
}

#endif // !__NENA_VIDEOFACELXND_INCLUDED__