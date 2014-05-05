#include "app.precompiled.h"

#ifndef __NENA_INTERACTIVE_TV_GPRAHICS_INCLUDED__
#define __NENA_INTERACTIVE_TV_GPRAHICS_INCLUDED__

namespace InteractiveTV
{
	struct Graphics 
	{
		Nena::Graphics::DeviceResources d3d;
		Nena::Graphics::OverlayResources d2d;

		Graphics();
		~Graphics();
		void Init();
	};
}

#endif // !__NENA_INTERACTIVE_TV_GPRAHICS_INCLUDED__
