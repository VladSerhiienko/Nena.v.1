#include "pch.h"
#include "GameRenderer.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;

static const ColorF BackgroundColors[] = 
{
	ColorF::CornflowerBlue,
	ColorF::Orchid,
	ColorF::DeepPink,
	ColorF::Red,
	ColorF::DarkOrange,
	ColorF::Gold,
	ColorF::GreenYellow,
	ColorF::Aqua
};

GameRenderer::GameRenderer() :
	//DirectXBase(D3D_DRIVER_TYPE_REFERENCE),
	DirectXBase(),
	m_clear(0.2f, 0.2f, 0.2f, 1.0f),
	m_textPosition(0.0f, 0.0f),
	m_backgroundColorIndex(0),
	m_renderNeeded(true)
{
}

void GameRenderer::CreateDeviceIndependentResources()
{
	DirectXBase::CreateDeviceIndependentResources();

	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextFormat(
			L"Segoe UI",
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			42.0f,
			L"en-US",
			&m_textFormat
			)
		);

	DX::ThrowIfFailed(
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
		);

	m_room.ReadFrom("Assets/Game/rm.spec.n.cave2.xml");
}

void GameRenderer::CreateDeviceResources()
{
	DirectXBase::CreateDeviceResources();

	DX::ThrowIfFailed(
		m_d2dContext->CreateSolidColorBrush(
			ColorF(ColorF::Black),
			&m_blackBrush
			)
		);

	Platform::String^ text = "Hello, DirectX!";

	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextLayout(
			text->Data(),
			text->Length(),
			m_textFormat.Get(),
			700, // maxWidth.
			1000, // maxHeight.
			&m_textLayout
			)
		);

	DX::ThrowIfFailed(
		m_textLayout->GetMetrics(&m_textMetrics)
		);
}

void GameRenderer::CreateWindowSizeDependentResources()
{
	DirectXBase::CreateWindowSizeDependentResources();

	m_roomRenderer = ref new RoomRenderer(
		m_d3dDevice.Get(), 
		m_d3dContext.Get(),
		m_wicFactory.Get(),
		this
		);

	m_roomRenderer->Height( _To_ushort m_d3dRenderTargetSize.Height);
	m_roomRenderer->Width( _To_ushort m_d3dRenderTargetSize.Width);
	m_roomRenderer->Near(0.001f);
	m_roomRenderer->Fov(XM_PIDIV4);
	m_roomRenderer->Far(200.0f);

	/*m_roomRenderer->EditCamera()->EditUp()->y = 1.0f;
	m_roomRenderer->EditCamera()->EditEye()->x = 20.0f;
	m_roomRenderer->EditCamera()->EditEye()->y = 20.0f;
	m_roomRenderer->EditCamera()->EditEye()->z = -20.0f;
	m_roomRenderer->EditCamera()->EditFocus()->x = 1.0f;
	m_roomRenderer->EditCamera()->EditFocus()->y = 2.0f;
	m_roomRenderer->EditCamera()->Fov(XM_PIDIV4);
	m_roomRenderer->EditCamera()->Far(100.0f);
	m_roomRenderer->EditCamera()->Near(0.01f);*/
	//m_roomRenderer->EditCamera()->Far(200.0f);



	// Buffers, textures and shaders (without any processing)
	m_roomRenderer->GenerateResources( &m_room );

	//auto eye = Reign::Vector3(0.10f, -10.5f, 10.10f);
	//auto foc = Reign::Vector3(0.0f, 0.0f, 0.0f);

	/*(*m_roomRenderer->EditView()) = Reign::Matrix::CreateLookAt(
		Reign::Vector3(3.10f, 10.0f, -1.10f),
		Reign::Vector3(0.0f, 0.0f, 0.0f),
		Reign::Vector3(0.0f, 1.0f, 0.0f)
		);

	(*m_roomRenderer->EditProjection()) = Reign::Matrix::CreatePerspectiveFieldOfView(
		XM_PIDIV4, m_d3dRenderTargetSize.Width / m_d3dRenderTargetSize.Height,
		0.010f, 
		100.0f
		);*/

	// Add code to create window size dependent objects here.
}

void GameRenderer::Update( _In_ GameRenderer::TimerView *timer )
{
	//(void) timeTotal; // Unused parameter.
	//(void) timeDelta; // Unused parameter.

	// Add code to update time dependent objects here.
}

void GameRenderer::Render()
{

   /* static const float32 clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	m_d3dContext->OMSetRenderTargets(
		1, m_d3dRenderTargetView.GetAddressOf(), 
		m_d3dDepthStencilView.Get()
		);

	m_d3dContext->ClearRenderTargetView(
		m_d3dRenderTargetView.Get(), 
		clearColor
		);

	m_d3dContext->ClearDepthStencilView(
		m_d3dDepthStencilView.Get(), 
		D3D11_CLEAR_DEPTH, 
		1.0f, 0ui8
		);
	*/

	m_roomRenderer->OnFrameMove();
	/*m_renderNeeded = */

	m_renderNeeded = true;
	m_roomRenderer->OnFrame();

	//EndFrame();

#if 0
	m_d2dContext->BeginDraw();

	//m_d2dContext->Clear(ColorF(BackgroundColors[m_backgroundColorIndex]));

	// Position the rendered text.
	Matrix3x2F translation = Matrix3x2F::Translation(
		m_windowBounds.Width / 2.0f - m_textMetrics.widthIncludingTrailingWhitespace / 2.0f + m_textPosition.X,
		m_windowBounds.Height / 2.0f - m_textMetrics.height / 2.0f + m_textPosition.Y
		);

	// Note that the m_orientationTransform2D matrix is post-multiplied here
	// in order to correctly orient the text to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain's target bitmap. For draw calls to other targets,
	// this transform should not be applied.
	m_d2dContext->SetTransform(translation * m_orientationTransform2D);

	m_d2dContext->DrawTextLayout(
		Point2F(0.0f, 0.0f),
		m_textLayout.Get(),
		m_blackBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
		);

	// Ignore D2DERR_RECREATE_TARGET. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = m_d2dContext->EndDraw();

	/*if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}*/

#endif

}

void GameRenderer::UpdateTextPosition(Point deltaTextPosition)
{
	m_textPosition.X += deltaTextPosition.X;
	m_textPosition.Y += deltaTextPosition.Y;
}

void GameRenderer::BackgroundColorNext()
{
	m_backgroundColorIndex++;
	if (m_backgroundColorIndex >= ARRAYSIZE(BackgroundColors))
	{
		m_backgroundColorIndex = 0;
	}
}

void GameRenderer::BackgroundColorPrevious()
{
	m_backgroundColorIndex--;
	if (m_backgroundColorIndex < 0)
	{
		m_backgroundColorIndex = ARRAYSIZE(BackgroundColors) - 1;
	}
}

void GameRenderer::SaveInternalState(IPropertySet^ state)
{
	if (state->HasKey("m_backgroundColorIndex"))
	{
		state->Remove("m_backgroundColorIndex");
	}
	if (state->HasKey("m_textPosition"))
	{
		state->Remove("m_textPosition");
	}
	state->Insert("m_backgroundColorIndex", PropertyValue::CreateInt32(m_backgroundColorIndex));
	state->Insert("m_textPosition", PropertyValue::CreatePoint(m_textPosition));
}

void GameRenderer::LoadInternalState(IPropertySet^ state)
{
	if (state->HasKey("m_backgroundColorIndex") && state->HasKey("m_textPosition"))
	{
		m_backgroundColorIndex = safe_cast<IPropertyValue^>(state->Lookup("m_backgroundColorIndex"))->GetInt32();
		m_textPosition = safe_cast<IPropertyValue^>(state->Lookup("m_textPosition"))->GetPoint();
	}
}
