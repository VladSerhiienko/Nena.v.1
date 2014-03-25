#include "Engine.h"
#include "VolumeRenderer.h"

#include <Nena\App.h>
#include "RaytraceAssist.h"

INT32 WINAPI WinMain(
	::HINSTANCE hInstance,
	::HINSTANCE hPrevInstance,
	::LPTSTR pScmdline,
	::INT32 iCmdshow
	)
{
	Nena::App *app = Nena::App::GetForCurrentThread();
	Demo::Engine *engine = Demo::Engine::GetForCurrentThread();

	/*Demo::Ray r;
	r.O.x = 0.0f;
	r.O.y = 0.0f;
	r.O.z = -5.0f;
	r.D.x = 0.0f;
	r.D.y = 0.0f;
	r.D.z = 1.0f;

	Nena::Vector3 aabb_min = { -2, -2, -2 };
	Nena::Vector3 aabb_max = { +2, +2, +2 };

	float distance;
	float t1, t2;

	Nena::Matrix obbw = Nena::Matrix::CreateRotationY(Nena::XM_PIDIV4) * Nena::Matrix::CreateTranslation(0, 0, 10);
	Nena::Matrix obbwi = obbw.Invert();

	bool test = Demo::RaytraceAssist::TestObb(
		r.O, r.D,
		aabb_min,
		aabb_max,
		obbw,
		distance, t1, t2
		);

	Nena::Vector3 startw = r(t1);
	Nena::Vector3 endw = r(t2);

	Nena::Vector3 starto = Nena::XMVector3Transform(startw, obbwi);
	Nena::Vector3 endo = Nena::XMVector3Transform(endw, obbwi);*/

	Demo::VolumeRenderer::Get()->CreateDeviceResources(nullptr);
	Demo::VolumeRenderer::Get()->Resource.Viewer.UpdateView({ 0, 0 }, 0);
	Demo::VolumeRenderer::Get()->Resource.Viewer.UpdateProjection();
	//Demo::VolumeRenderer::Get()->Resource.ObbTransform.EvaluateWorld();

	app->MessageLoop();

	return 0;
}