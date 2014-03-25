#include "DirectXPch.h"

#ifndef __NENA_D3DRTBASE_H__
#define __NENA_D3DRTBASE_H__

namespace Nena
{
	namespace Graphics
	{
		// Direct3D rendering objects. Required for 3D.
		struct DeviceRenderTargetBase
		{

			Nena::Graphics::DeviceRenderTargetBase::DeviceRenderTargetBase() : Viewport(), ClearColor()
			{
				ClearColor.A(1.0f);
			}

			Resources::ViewportDescription Viewport;
			Resources::RTView RenderTargetView;
			Resources::DSView DepthStencilView;
			Nena::Color ClearColor;

			// Clears render target view, depth stencil view.
			inline void Nena::Graphics::DeviceRenderTargetBase::Clear(_In_ Resources::IContext3 *context)
			{
				context->ClearRenderTargetView(
					RenderTargetView.Get(),
					ClearColor
					);
				context->ClearDepthStencilView(
					DepthStencilView.Get(),
					D3D11_CLEAR_DEPTH,
					1.0f, 0ui8
					);
			}

			// Sets render target view, depth stencil view and viewport to context.
			inline void Nena::Graphics::DeviceRenderTargetBase::BindToPipeline(
				_In_ Resources::IContext3 *context
				)
			{
				context->OMSetRenderTargets(
					1, RenderTargetView.GetAddressOf(),
					DepthStencilView.Get()
					);
				context->RSSetViewports(
					1, &Viewport
					);
			}

			// Sets render target view, depth stencil view to null.
			inline void Nena::Graphics::DeviceRenderTargetBase::UnbindFromPipeline(
				_In_ Resources::IContext3 *context
				)
			{
				context->OMSetRenderTargets(
					0, &Resources::NullResources::NullRTV,
					Resources::NullResources::NullDSV
					);
			}
		};
	}
}

#endif // !__NENA_D3DRTBASE_H__
