
#include "VideoUtilityPerc.h"

namespace Nena
{
	namespace Video
	{
		UINT32 Utility::GetStreamCountFromDesc(
			PXCCapture::VideoStream::DataDesc &desc
			)
		{
			return GetStreamCountFromDesc(&desc);
		}

		UINT32 Utility::GetStreamCountFromDesc(
			PXCCapture::VideoStream::DataDesc *desc
			)
		{
			pxcU32 formatIndex = 0;
			for (; desc && formatIndex < ARRAYSIZE(desc->streams); formatIndex++)
			if (!desc->streams[formatIndex].format) break;
			return formatIndex;
		}
	}
}