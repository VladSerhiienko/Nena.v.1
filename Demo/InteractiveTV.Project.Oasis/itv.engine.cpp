#include "app.precompiled.h"
#include "itv.engine.h"

InteractiveTV::Engine::Engine() 
{
}


void InteractiveTV::Engine::Init()
{
	Input.Init();
	Graphics.Init();
}

void InteractiveTV::Engine::Quit()
{
	//! @todo Graphics.Quit
}

InteractiveTV::Engine *InteractiveTV::Engine::GetForCurrentThread()
{
	static Engine s_engine; return &s_engine;
}

void InteractiveTV::Engine::BeginScene()
{
	Graphics.d3d.Clear(Graphics.d3d.Context.Get());
	Graphics.d3d.BindToPipeline(Graphics.d3d.Context.Get());

}
void InteractiveTV::Engine::EndScene()
{
	::HRESULT result(S_OK);

	result = Graphics.d3d.Present();
	Graphics.d3d.UnbindFromPipeline(Graphics.d3d.Context.Get());
}
