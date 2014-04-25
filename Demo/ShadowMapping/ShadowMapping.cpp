#include "ShadowMapping.h"
#include "ContentLoader.h"

#include <Nena\AppVars.h>

Demo::ShadowMapping *Demo::ShadowMapping::Get()
{
	static ShadowMapping s_demo;
	return &s_demo;
}

Demo::ShadowMapping::ShadowMapping()
{
	Cube.Name = "Cube";
	Sphere.Name = "Sphere";
	Ground.Name = "Ground";

	MLBPressed = FALSE;
	MRBPressed = FALSE;
	MLPos.x = MLPos.y = 0.0f;
	MRPos.x = MRPos.y = 0.0f;
	MLDlt.x = MLDlt.y = 0.0f;
	MRDlt.x = MRDlt.y = 0.0f;
}

Demo::ShadowMapping::~ShadowMapping()
{
}

void Demo::ShadowMapping::HardValidation()
{
	HRESULT result = S_OK;

	Nena::App::Vars *vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	_ASSERT_EXPR(SUCCEEDED(result), 
		L"Cannot find app vars file");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/File"),
		L"Cannot find record: scene file");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Ground/Transform"),
		L"Cannot find record: ground shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Sphere/Transform"),
		L"Cannot find record: sphere shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Cube/Transform"),
		L"Cannot find record: cube shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Ground/Texture"),
		L"Cannot find record: ground shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Sphere/Texture"),
		L"Cannot find record: sphere shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Cube/Texture"),
		L"Cannot find record: cube shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Ground/Shape"),
		L"Cannot find record: ground shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Sphere/Shape"),
		L"Cannot find record: sphere shape name");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Scene/Import/Cube/Shape"),
		L"Cannot find record: cube shape name");

	Depth.HardValidation();
	Shadow.HardValidation();
}

void  Demo::ShadowMapping::CreateDeviceResources()
{
	LightResources.CreateDeviceResources();
	CameraResources.CreateDeviceResources();
	for (auto &shape : Resources) 
		shape->CreateDeviceResources();
	Depth.CreateDeviceResources();
	Shadow.CreateDeviceResources(&Depth);
}

void Demo::ShadowMapping::UploadResources()
{
	HardValidation();

	HRESULT result = S_OK;
	Nena::App::Vars *vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	ContentLoader loader;
	loader.UploadShape(
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/File"),
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/Cube/Shape"),
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/Cube/Transform"),
		&Cube
		);
	loader.UploadShape(
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/File"),
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/Sphere/Shape"),
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/Sphere/Transform"),
		&Sphere
		);
	loader.UploadShape(
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/File"),
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/Ground/Shape"),
		vars->GetRecord("Demo/ShadowMapping/Scene/Import/Ground/Transform"),
		&Ground
		);

	Resources.push_back(&CubeResources);
	Resources.push_back(&SphereResources);
	Resources.push_back(&GroundResources);

	CubeResources.Shape = &Cube;
	SphereResources.Shape = &Sphere;
	GroundResources.Shape = &Ground;

	CubeResources.Viewer = &Camera;
	SphereResources.Viewer = &Camera;
	GroundResources.Viewer = &Camera;

	CubeResources.Light = &Light;
	SphereResources.Light = &Light;
	GroundResources.Light = &Light;

	CubeResources.Shape->TransformOS.EvaluateWorld();
	SphereResources.Shape->TransformOS.EvaluateWorld();
	GroundResources.Shape->TransformOS.EvaluateWorld();

	Depth.Meshes = &Resources;
	Shadow.Meshes = &Resources;
	Depth.Light = &LightResources;
	Shadow.Light = &LightResources;
	Depth.Camera = &CameraResources;
	Shadow.Camera = &CameraResources;

	Camera.Displacement.x = -15.0f;
	Camera.Displacement.y = 20.0f;
	Camera.Displacement.z = -10.0f;

	Light.FieldOfView = Nena::XM_PIDIV2;
	Light.Displacement.x = 1.0f;
	Light.Displacement.y = 15.0f;
	Light.Displacement.z = 1.0f;
	Light.AspectRatio = 1.0f;

	Camera.UpdateView({ 0, 0 }, 0);
	Light.UpdateView({ 0, 0 }, 0);
	Camera.UpdateProjection();
	Light.UpdateProjection();
}

void Demo::ShadowMapping::Update()
{
	Depth.Update();
	Shadow.Update();
}

void Demo::ShadowMapping::Render()
{
	Depth.Render();
	Shadow.Render(&Depth);
}

