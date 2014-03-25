#include "Video.h"

namespace Nena
{
	namespace Video
	{
		Sample::~Sample()
		{
			for (UINT32 i = 0; i < MaxStreams; i++)
			if (Images[i]) Images[i]->Release(),
				SyncsInter[i] = NULL;
			else break;

			SyncOut = NULL;
		}

		void Sample::ReleaseImages()
		{
			for (UINT32 i = 0; i < MaxStreams; i++)
			if (Images[i]) Images[i]->Release();
			else break;
		}

		Sample::Status Sample::SynchronizeSyncsInter(UINT32(&idx)[MaxStreams])
		{
			return SyncsInter[0]->SynchronizeEx(StreamCount, &SyncsInter[0], idx);
		}

		Sample::Status Sample::SynchronizeSyncsInter()
		{
			return SyncsInter[0]->SynchronizeEx(StreamCount, &SyncsInter[0]);
		}

		Sample::Status Sample::OnSubmit_Choose(Sample *sample)
		{
			sample->OnSubmitCallback = sample->StreamCount > 1
				? OnSubmit_MultipleStream : OnSubmit_SingleStream;
			return sample->OnSubmitCallback(sample);
		}

		Sample::Status Sample::OnSubmit_SingleStream(Sample *sample)
		{
			return sample->VideoStreams[0]->ReadStreamAsync(
				&sample->Images[0], &sample->SyncOut
				);
		}

		Sample::Status Sample::OnSubmit_MultipleStream(Sample *sample)
		{
			typedef PXCSmartAsyncImplIxOy<Sample, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT,
				PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT> AsyncImpl;

			PXCSmartSPArray syncs(Sample::MaxStreams);

			UINT32 streamIndex = 0;
			Sample::Status statuses[Sample::MaxStreams];
			for (; streamIndex < sample->StreamCount; streamIndex++)
				statuses[streamIndex] = sample->VideoStreams[streamIndex]->ReadStreamAsync(
				&sample->Images[streamIndex],
				&syncs[streamIndex]
				);

			pxcStatus onSubmitTask = AsyncImpl::SubmitTask(
				sample->Images, sample->StreamCount, // images
				syncs.ReleasePtrs(), sample->StreamCount, // syncs
				&sample->SyncOut, sample, // out sync
				sample->Host->Scheduler, // sync scheduler
				&Sample::ReadSync, // sync callback
				&Sample::PassOnStatus, // status callback
				//L"Sample::OnSubmit_MultipleStream()"
				nullptr // name
				//L""
				);

			return onSubmitTask;
		}

		Sample::Sample(Capture *host, UINT32 streamCount) :
			StreamCount(streamCount > MaxStreams ? MaxStreams : streamCount),
			Host(host),
			SyncOut()
		{
			for (UINT32 i = 0; i < MaxStreams; i++)
				StreamIndices[i] = UINT_ERROR,
				VideoStreams[i] = NULL,
				Images[i] = NULL,
				SyncsInter[i] = NULL;

			OnSubmitCallback = &OnSubmit_Choose;
			OnSubmitCallbackEx = &OnSubmitEx_Choose;
			SyncOut = NULL;
		}

		Sample::Status PXCAPI Sample::ReadSync(PXCImage *images[], PXCScheduler::SyncPoint *spts[])
		{
			//Platform::DebugTrace("\t >> Sampler::ReadSync() [Size=%u]\n", StreamCount);
			if (spts[0]) spts[0]->SynchronizeEx(StreamCount, spts); // synchronize
			for (pxcU32 s = 0; s < StreamCount; s++) /*if (spts[s])*/ spts[s]->Release(); // release
			//Platform::DebugTrace("\t << Sampler::ReadSync()\n", StreamCount);

			return PXC_STATUS_NO_ERROR; // ok
		}

		Sample::Status PXCAPI Sample::PassOnStatus(pxcStatus sts)
		{
			//Platform::DebugTrace("\t >< Sampler::PassOnStatus() [Status=%u]\n", sts);
			return sts;
		}

		Sample::Status Sample::OnSubmitEx_Choose(Sample *sample, PXCImage *images[], PXCScheduler::SyncPoint **sync)
		{
			sample->OnSubmitCallbackEx = sample->StreamCount > 1
				? OnSubmitEx_MultipleStream : OnSubmitEx_SingleStream;
			return sample->OnSubmitCallbackEx(sample, images, sync);
		}

		Sample::Status Sample::OnSubmitEx_SingleStream(Sample *sample, PXCImage **image, PXCScheduler::SyncPoint **sync)
		{
			return sample->VideoStreams[0]->ReadStreamAsync(image, sync);
		}

		Sample::Status Sample::OnSubmitEx_MultipleStream(Sample *sample, PXCImage *images[], PXCScheduler::SyncPoint **sync)
		{
			typedef PXCSmartAsyncImplIxOy<Sample, PXCImage, PXCCapture::VideoStream::STREAM_LIMIT,
				PXCScheduler::SyncPoint, PXCCapture::VideoStream::STREAM_LIMIT> AsyncImpl;

			PXCSmartSPArray syncs(Sample::MaxStreams);

			UINT32 streamIndex = 0;
			Sample::Status statuses[Sample::MaxStreams];
			for (; streamIndex < sample->StreamCount; streamIndex++)
				statuses[streamIndex] = sample->VideoStreams[streamIndex]->ReadStreamAsync(
				&images[streamIndex],
				&syncs[streamIndex]
				);

			pxcStatus onSubmitTask = AsyncImpl::SubmitTask(
				images, sample->StreamCount, // images
				syncs.ReleasePtrs(), sample->StreamCount, // syncs
				sync, sample, // out sync
				sample->Host->Scheduler, // sync scheduler
				&Sample::ReadSync, // sync callback
				&Sample::PassOnStatus, // status callback
				L"Sample::OnSubmitEx_MultipleStream()"
				//nullptr // name
				//L""
				);

			return onSubmitTask;
		}

		Sample::Status Sample::SubmitReadTask()
		{
			return OnSubmitCallback(this);
		}


		Sample::Status Sample::SubmitReadTask(
			PXCImage *images[], PXCScheduler::SyncPoint **sync
			)
		{
			return OnSubmitCallbackEx(this, images, sync);
		}

		Sample::Status Sample::AwaitReadTask()
		{
			Sample::Status status = SyncOut->Synchronize();
			SyncOut->Release();
			return status;
		}

		Sample * Capture::GetNewSample(UINT32 streamCount, UINT32 *out)
		{
			if (streamCount > Sample::MaxStreams)
				Platform::DebugTrace(
				"WARNING [Capture::GetNewSample] "
				"Stream count should be less equal %u "
				"(was automatically clamped)\n",
				Sample::MaxStreams
				);
			streamCount = streamCount > Sample::MaxStreams
				? Sample::MaxStreams : streamCount;

			Sample *newSample = new Sample(this, streamCount);

			if (out) (*out) = Samples.size();
			Samples.push_front(newSample);
			return newSample;
		}

		BOOL Capture::GetSampleFromSelf(Sample **sampleRef)
		{
			Sample *sample = nullptr;

			if (!sampleRef) return FALSE; // null address
			if (!(sample = *sampleRef)) return FALSE; // null pointer
			if (sample->StreamCount > Sample::MaxStreams) return FALSE; // to much streams
			if (sample->StreamCount > VideoStreams.size()) return FALSE; // to much streams
			if (sample->StreamIndices[0] == UINT_ERROR) return FALSE; // at least one stream is expected

			UINT32 streamIndex = 0;
			for (; streamIndex < sample->StreamCount; streamIndex++)
			if (sample->StreamIndices[streamIndex] != UINT_ERROR &&
				sample->StreamIndices[streamIndex] < VideoStreams.size())
				sample->VideoStreams[streamIndex] =
				VideoStreams[sample->StreamIndices[streamIndex]];
			else return FALSE;

			return TRUE;
		}

		Sample * Capture::GetSampleFromDesc(UINT32 desc)
		{
			UINT32 streamCount = 0, stream = 0;
			if (desc < VideoDescs.size())
			if (streamCount = Utility::GetStreamCountFromDesc(VideoDescs[desc]))
			{
				UINT32 out;
				Sample *newSample = GetNewSample(streamCount, &out);
				for (; stream < streamCount; stream++)
					newSample->StreamIndices[stream] =
					VideoMapping[desc].u32[stream];
				if (GetSampleFromSelf(&newSample))
					return newSample;
				else Samples.remove(newSample),
					delete newSample;
			}

			return nullptr;
		}
	}
}