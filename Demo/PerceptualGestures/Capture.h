
#include "Precompiled.h"

#ifndef __NENA_PERCEPTUAL_VIDEOCAPTURE_INCLUDED__
#define __NENA_PERCEPTUAL_VIDEOCAPTURE_INCLUDED__

namespace Nena
{
	namespace Video
	{
		struct Pipe;

		struct Capture
		{
			Pipe *Host;

			PXCSessionService *Service;
			PXCSmartPtr<PXCScheduler> Scheduler;
			PXCSmartPtr<PXCCapture> Base;
			PXCSmartPtr<PXCCapture::Device> Device;

			PXCSession::ImplDesc CaptureFilter;
			PXCCapture::DeviceInfo DeviceFilter;

			Capture(Pipe *pipe = nullptr);
			HRESULT Init();
			void Close();

		protected:

			PXCSession::ImplDesc m_captureFilter;
			PXCCapture::DeviceInfo m_deviceFilter;

		};

	}

}

#endif // !__NENA_PERCEPTUAL_VIDEOCAPTURE_INCLUDED__

