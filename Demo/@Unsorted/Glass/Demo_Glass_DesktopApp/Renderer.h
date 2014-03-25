#pragma once
#include "SceneRenderer.h"
#include "GlassRenderer.h"
#include "Camera.h"


namespace Sandbox
{
	ref_class_decl Renderer : public Sehnsucht::DirectXBaseDesktop
	{
		typedef CRITICAL_SECTION CriticalSection;

	protected_shared_access:

		void CreateDeviceResources(_No_args_);
		void CreateDeviceIndependentResources(_No_args_);
		void CreateWindowSizeDependentResources(_No_args_);

	public_shared_access:

		Renderer();
		void Initialize();
		void UpdateForWindowSizeChange();

		REIGN_FORCEINLINING void OnFrameMove(_In_ Sehnsucht::BasicTimerViewPtr timer)
		{ // Update time dependent values
			EnterCriticalSection(&m_lock);

			m_sceneRenderer->EditSceneSpatial()->World = Matrix::CreateScale(2.0f) *
				Matrix::CreateTranslation(0.0f, 0.0f, 5.0f) *
				Matrix::CreateRotationY(timer->Elapsed) *
				Matrix::CreateRotationX(timer->Elapsed) *
				Matrix::CreateTranslation(0.0f, 0.0f, -10.0f);

			m_sceneRenderer->OnFrameMove(timer);
			m_glassRenderer->OnFrameMove(timer);
			LeaveCriticalSection(&m_lock);
		}

		REIGN_FORCEINLINING void OnFrame(_No_args_)
		{ // Pipeline reconfig + Draw calls
			static const float32 s_factor [] = { 0.0f, 0.0f, 0.0f, 0.0f };
			static const auto s_sample = 0xffffffff;
			static const auto s_offset = 0u;

			// Lock
			EnterCriticalSection(&m_lock);

			// -------------------------------------------------------
			// Render scene
			m_sceneRenderTarget.BindToPipeline(m_d3dContext.Get());
			m_sceneRenderTarget.Clear(m_d3dContext.Get());

			m_d3dContext->RSSetState(m_rasterizerCcw.Get());
			//m_d3dContext->OMSetBlendState(m_blendingEnabled.Get(), s_factor, s_sample);

			m_sceneRenderer->OnFrame();
			m_sceneRenderTarget.UnbindFromPipeline(m_d3dContext.Get());
			// -------------------------------------------------------
			// Render glass
			BindToPipeline(m_d3dContext.Get());
			Clear(m_d3dContext.Get());

			m_d3dContext->RSSetState(m_rasterizerCcw.Get());
			m_d3dContext->OMSetBlendState(m_blendingEnabled.Get(), s_factor, s_sample);

			//m_sceneRenderer->OnFrame();
			m_sceneRenderer->OnFrame();
			m_glassRenderer->OnFrame();
			PresentVSync();
			UnbindFromPipeline(m_d3dContext.Get());
			// -------------------------------------------------------
			// Unlock
			LeaveCriticalSection(&m_lock);
		}

	public_dtor_access:

		~Renderer(_No_args_);

	protected_shared_access:

		Sandbox::EventArgsForWindowSizeChange m_windowSizeChangedArgs;
		Sehnsucht::BasicRenderTarget m_sceneRenderTarget;
		Rendering::RasterizerState m_rasterizerCcw;
		Rendering::BlendState m_blendingEnabled;
		SceneRenderer *m_sceneRenderer;
		GlassRenderer *m_glassRenderer;
		Camera m_camera;

		// Synchronization
		CriticalSection m_lock;
	};
};