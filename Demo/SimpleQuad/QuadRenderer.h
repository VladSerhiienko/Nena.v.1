#pragma once

#pragma once

#include "QuadMesh.h"
#include "QuadResources.h"

namespace Framework
{
	namespace Application
	{
		struct Window;
		struct QuadRenderer;
		struct QuadRendererNotify : Nena::Graphics::IDeviceNotify
		{
			virtual void OnDeviceLost() override;
			virtual void OnDeviceRestored() override;

			QuadRenderer *Target;
		};

		struct QuadRenderer
		{
			friend QuadRendererNotify;

			// requires device resources
			QuadRenderer(Window *wnd);

			QuadMesh Mesh; // data needed to render square
			QuadResources Resources; // resources needed to render square / device resources
			::HRESULT LastError; // check to see if this instance is valid
			::BOOL PendingChanges;

			// required for renderings

			Nena::Graphics::Resources::DepthStencilState DSs; // depth stencil / device resources
			Nena::Graphics::Resources::RasterizerState Rs; // rasterizer / device resources

			::HRESULT CreateDeviceDependentResources();
			::HRESULT CreateDeviceIndependentResources();
			::HRESULT CreateWindowSizeDependentResources();
			::HRESULT Update();
			::HRESULT Render();

		private:

			Nena::Graphics::DeviceResources *D3DResources;
			inline Nena::Graphics::Resources::IDirect3DDevice *GetD3DDevice() { return D3DResources->Device.Get(); }
			inline Nena::Graphics::Resources::IDirect3DContext *GetD3DContext() { return D3DResources->Context.Get(); }

		};
	}
}