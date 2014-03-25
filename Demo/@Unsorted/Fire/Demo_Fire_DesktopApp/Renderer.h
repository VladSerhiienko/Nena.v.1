#pragma once
#include "FireRenderer.h"

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
			m_fireRenderer->OnFrameMove(timer);
			LeaveCriticalSection(&m_lock);
		}

		REIGN_FORCEINLINING void OnFrame(_No_args_)
		{ // Pipeline reconfig + Draw calls
			EnterCriticalSection(&m_lock);

			BindToPipeline(m_d3dContext.Get());
			m_d3dContext->ClearRenderTargetView(
				m_d3dRenderTargetView.Get(),
				reinterpret_cast<float32*>(&m_clear)
				);

			m_d3dContext->ClearDepthStencilView(
				m_d3dDepthStencilView.Get(),
				D3D11_CLEAR_DEPTH,
				1.0f, 0ui8
				);

			if (m_fireRenderer->OnFrame())
				PresentVSync();

			LeaveCriticalSection(&m_lock);
		}

	public_dtor_access:

		~Renderer(_No_args_);

	protected_shared_access:

		FireRenderer *m_fireRenderer;
		Reign::Vector4 m_clear;

		CriticalSection m_lock;
		bool m_busy;
	};
};