
#include "Shape.h"
#include "ShapeResources.h"
#include "CameraResources.h"
#include "LightResources.h"
#include "DepthRenderer.h"
#include "ShadowRenderer.h"

namespace Demo
{

	struct ShadowMapping
	{
		BOOL MLBPressed;
		BOOL MRBPressed;
		Nena::Vector2 MLPos;
		Nena::Vector2 MRPos;
		Nena::Vector2 MLDlt;
		Nena::Vector2 MRDlt;

		Demo::Shape Cube;
		Demo::Shape Sphere;
		Demo::Shape Ground;
		Demo::Light Light;
		Demo::Camera Camera;

		Demo::ShapeResources CubeResources;
		Demo::ShapeResources SphereResources;
		Demo::ShapeResources GroundResources;
		Demo::LightResources LightResources;
		Demo::CameraResources CameraResources;
		Demo::ShapeResources::Vector Resources;

		Demo::DepthRenderer Depth;
		Demo::ShadowRenderer Shadow;

		void CreateDeviceResources();
		void UploadResources();
		void Update();
		void Render();

		static ShadowMapping *Get();
		~ShadowMapping();

	protected:

		ShadowMapping();
		void HardValidation();
	};

}

