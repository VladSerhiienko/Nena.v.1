#include "app.precompiled.h"
#include "itv.graphics.h"
#include "itv.input.h"

#ifndef __NENA_INTERACTIVE_TV_ENGINE_INCLUDED__
#define __NENA_INTERACTIVE_TV_ENGINE_INCLUDED__

namespace InteractiveTV
{
	struct Engine
	{
		Input Input;
		Graphics Graphics;

		Engine();
		void Init();
		void Quit();
		void BeginScene();
		void EndScene();
		void PresentScene();


	private:

		friend InteractiveTV::Project::Oasis;
		static Engine *GetForCurrentThread();
	};
}

#endif // !__NENA_INTERACTIVE_TV_ENGINE_INCLUDED__
