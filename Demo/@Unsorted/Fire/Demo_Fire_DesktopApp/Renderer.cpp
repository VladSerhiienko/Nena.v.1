#include "Precompiled.h"
#include "Renderer.h"

Sandbox::Renderer::Renderer() : m_clear(0.05f, 0.05f, 0.05f, 1.0f), m_busy(true)
{
	PrintToDebugWindowOnly8("Sandbox. Creating renderer.");
	m_fireRenderer = alloc_ref_new FireRenderer();

	InitializeCriticalSection(&m_lock);
}

Sandbox::Renderer::~Renderer()
{
	PrintToDebugWindowOnly8("Sandbox. Deleting renderer.");

	DeleteCriticalSection(&m_lock);
	// In runtime when the object falls out of scope, it is automatically deleted.
#ifdef REIGN_DESKTOP // Desktop implementation
	Safe_Delete(m_fireRenderer);
#endif // All implementations
}

void Sandbox::Renderer::CreateDeviceResources()
{
	PrintToDebugWindowOnly8("Sandbox. Creating device resources.");
}

void Sandbox::Renderer::CreateDeviceIndependentResources()
{
	PrintToDebugWindowOnly8("Sandbox. Creating device independent resources.");
}

void Sandbox::Renderer::CreateWindowSizeDependentResources()
{
	PrintToDebugWindowOnly8("Sandbox. Creating window size dependent resources.");
}

void Sandbox::Renderer::Initialize()
{
	EnterCriticalSection(&m_lock);
	m_busy = true;

	PrintToDebugWindowOnly8("Sandbox. Initializing renderer.");

	DirectXBaseDesktop::Initialize();
	m_fireRenderer->WICFactory(m_wicFactory.Get());
	m_fireRenderer->Context3(m_d3dContext.Get());
	m_fireRenderer->Device3(m_d3dDevice.Get());
	m_fireRenderer->Initialize();

	m_fireRenderer->EditFireView()->Aspect(m_d3dRenderTargetSize.width / m_d3dRenderTargetSize.height);
	(*m_fireRenderer->EditFireView()->EditFocus()) = Reign::Vector3(0.0f, 0.0f, 5.0f);
	(*m_fireRenderer->EditFireView()->EditEye()) = Reign::Vector3(0.0f, 0.0f, -20.0f);
	(*m_fireRenderer->EditFireView()->EditUp()) = Reign::Vector3(0.0f, 1.0f, 0.0f);
	m_fireRenderer->EditFireView()->UpdateView();
	m_fireRenderer->EditFireView()->UpdateProj();

	ZeroValue(m_fireRenderer->EditFireDistortion());
	m_fireRenderer->EditFireDistortion()->Distortion0 = Reign::Vector2(0.101f, 0.2f);
	m_fireRenderer->EditFireDistortion()->Distortion1 = Reign::Vector2(0.02f, 0.3f);
	m_fireRenderer->EditFireDistortion()->Distortion2 = Reign::Vector2(0.13f, 0.1f);
	m_fireRenderer->EditFireDistortion()->ScaleBias = Reign::Vector2(0.8f, 0.5f);

	ZeroValue(m_fireRenderer->EditFireSpatial());
	m_fireRenderer->EditFireSpatial()->World = Reign::Matrix::CreateTranslation(0.0f, 0.0f, 5.0f);
	m_fireRenderer->EditFireSpatial()->View = m_fireRenderer->GetFireView()->GetViewCpy();
	m_fireRenderer->EditFireSpatial()->Proj = m_fireRenderer->GetFireView()->GetProjCpy();

	ZeroValue(m_fireRenderer->EditFireNoise());
	m_fireRenderer->EditFireNoise()->ScrollSpeeds = Reign::Vector3(1.3f, 2.1f, 2.3f);
	m_fireRenderer->EditFireNoise()->Scales = Reign::Vector3(1.0f, 2.0f, 3.0f) * 0.2f;

	m_busy = false;
	LeaveCriticalSection(&m_lock);
}

void Sandbox::Renderer::UpdateForWindowSizeChange()
{
	EnterCriticalSection(&m_lock);
	m_busy = true;

	PrintToDebugWindowOnly8("Sandbox. Updating renderer on window size changed.");
	DirectXBaseDesktop::UpdateForWindowSizeChange();

	m_fireRenderer->EditFireView()->Aspect(m_d3dRenderTargetSize.width / m_d3dRenderTargetSize.height);
	m_fireRenderer->EditFireView()->UpdateProj();

	m_fireRenderer->UpdateForWindowSizeChange();

	m_busy = false;
	LeaveCriticalSection(&m_lock);
}