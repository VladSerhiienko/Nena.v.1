#pragma once

#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

namespace Framework
{
	namespace Application
	{
		struct Window;
		struct TextRenderer
		{
			Framework::Application::TextRenderer::TextRenderer(
				Framework::Application::Window *window = nullptr
				);

			::HRESULT Framework::Application::TextRenderer::CreateDeviceIndependentResources();
			::HRESULT Framework::Application::TextRenderer::CreateDeviceDependentResources();
			::HRESULT Framework::Application::TextRenderer::ReleaseDeviceDependentResources();
			::HRESULT Framework::Application::TextRenderer::Update();
			::HRESULT Framework::Application::TextRenderer::Render();

			Nena::Graphics::Resources::Direct2DDrawingStateBlock State;
			Nena::Graphics::Resources::DirectWriteTextMetrics Metrics;
			Nena::Graphics::Resources::Direct2DSolidColorBrush Brush;
			Nena::Graphics::Resources::DirectWriteTextLayout Layout;
			Nena::Graphics::Resources::DirectWriteTextFormat Format;

			Nena::Graphics::OverlayResources *Overlay;
			Nena::Graphics::Resources::SizeF *LogicalSize;

			std::wstring Text;
			::BOOL Valid;

		};
	}
}
