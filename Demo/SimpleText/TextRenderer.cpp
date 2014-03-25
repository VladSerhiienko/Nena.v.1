#include "TextRenderer.h"
#include "Framework.h"


Framework::Application::TextRenderer::TextRenderer(
	Framework::Application::Window *window // nullptr
	) : Overlay(nullptr), 
		Text(L"Nena"),
		LogicalSize(nullptr),
		Metrics({ 0 }),
		Valid(FALSE)
{
	if (window)
	{
		LogicalSize = &window->Graphics.LogicalSize;
		Overlay = &window->Overlay;
	}
}

::HRESULT Framework::Application::TextRenderer::CreateDeviceIndependentResources()
{
	::HRESULT result = S_OK;

	using namespace Nena::Graphics;

	result = Overlay->DWFactory->CreateTextFormat(
		//L"Consolas", nullptr,
		//L"Segoe UI Light", nullptr,
		L"Broadway", nullptr,
		Resources::DirectWriteFontWeight::DWRITE_FONT_WEIGHT_LIGHT,
		Resources::DirectWriteFontStyle::DWRITE_FONT_STYLE_NORMAL,
		Resources::DirectWriteFontStretch::DWRITE_FONT_STRETCH_NORMAL,
		256.0f, L"en-US", &Format
		);
	if (FAILED(result)) return result;

	result = Format->SetTextAlignment(
		Resources::DirectWriteTextAlignment::DWRITE_TEXT_ALIGNMENT_CENTER
		);
	if (FAILED(result)) return result;

	result = Format->SetParagraphAlignment(
		Resources::DirectWriteParagraphAlignment::DWRITE_PARAGRAPH_ALIGNMENT_CENTER
		);
	if (FAILED(result)) return result;

	result = Overlay->D2DFactory->CreateDrawingStateBlock(
		&State
		);

	return result;
}

::HRESULT Framework::Application::TextRenderer::CreateDeviceDependentResources()
{
	::HRESULT result = S_OK;

	result = Overlay->Context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::ForestGreen), &Brush
		);

	return result;
}

::HRESULT Framework::Application::TextRenderer::ReleaseDeviceDependentResources()
{
	::HRESULT result = S_OK;
	Brush.Reset();
	return result;
}

::HRESULT Framework::Application::TextRenderer::Update()
{
	static ::HRESULT result = S_OK;

	result = Overlay->DWFactory->CreateTextLayout(
		Text.c_str(),
		(UINT32) Text.length(),
		Format.Get(),
		LogicalSize->Width,
		LogicalSize->Height,
		&Layout
		);
	if (FAILED(result)) return result;

	result = Layout->GetMetrics(&Metrics);

	return result;
}

::HRESULT Framework::Application::TextRenderer::Render()
{
	static ::HRESULT result = S_OK;
	static D2D1_POINT_2F TopLeft = D2D1::Point2F(0, 0);

	Overlay->Context->SaveDrawingState(State.Get());
	Overlay->Context->BeginDraw();

	D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(
		LogicalSize->Width - Metrics.layoutWidth,
		LogicalSize->Height - Metrics.layoutHeight
		);

	Overlay->Context->SetTransform(translation * Overlay->OrientationXform2D);
	Overlay->Context->DrawTextLayout(TopLeft, Layout.Get(), Brush.Get());

	result = Overlay->Context->EndDraw();
	if (FAILED(result) && result != D2DERR_RECREATE_TARGET) return result;
	Overlay->Context->RestoreDrawingState(State.Get());

	return result;
}


