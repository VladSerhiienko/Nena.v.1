#include "Precompiled.h"

#ifndef __NENA_PERCEPTUAL_VIDEOPIPE_INCLUDED__
#define __NENA_PERCEPTUAL_VIDEOPIPE_INCLUDED__

namespace Nena
{
	namespace Video
	{
		struct Pipe
		{
			PXCSmartPtr<PXCSession> Session;

			Pipe();
			HRESULT Init();
			void Close();

		};

	}
}

#endif // !__NENA_PERCEPTUAL_VIDEOPIPE_INCLUDED__

