#pragma once

#include "pch.h"
#include "DirectXBase.h"
#include "Room.h"
#include "RoomRenderer.h"

// This class renders simple text with a colored background.
ref class GameRenderer sealed : public DirectXBase
{
#pragma region Timer
	typedef Application::Utility::Timing::TimerView TimerView;
#pragma endregion

public:

	GameRenderer();

	// DirectXBase methods.
	virtual void CreateDeviceIndependentResources() override;
	virtual void CreateDeviceResources() override;
	virtual void CreateWindowSizeDependentResources() override;
	virtual void Render() override;

	// Method to change the text position based on input events.
	void UpdateTextPosition(Windows::Foundation::Point deltaTextPosition);

	// Methods to adjust the window background color.
	void BackgroundColorNext();
	void BackgroundColorPrevious();

	// Methods to save and load state in response to suspend.
	void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
	void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);
	
internal:

	// Method for updating time-dependent objects.
	void Update( GameRenderer::TimerView *timer );


	REIGN_FORCEINLINING Reign::Vector4 const *GetClear() const {  return &m_clear; };
	REIGN_FORCEINLINING Reign::Vector4 GetClearCpy() const { return m_clear; };
	REIGN_FORCEINLINING Reign::Vector4 *EditClear() { return &m_clear; };

	REIGN_FORCEINLINING void BeginFrame()
	{
		m_d3dContext->ClearRenderTargetView(
			m_d3dRenderTargetView.Get(), 
			reinterpret_cast<float32*>(&m_clear)
			);

		m_d3dContext->ClearDepthStencilView(
			m_d3dDepthStencilView.Get(), 
			D3D11_CLEAR_DEPTH, 
			1.0f, 0ui8
			);
	}

	void OnConnectionChanged(boolean connected)
	{
		m_roomRenderer->AttachTo(
			ClientContext::Get()
			);
	}

	REIGN_FORCEINLINING void EndFrame()
	{
		if (m_renderNeeded)
		{
			PrintToDebugWindowOnly8("Present");
			Present();
		}
	}
	
	REIGN_FORCEINLINING void Orientation( _In_ float32 value ) { m_roomRenderer->Orientation(value); }
	REIGN_FORCEINLINING float32 Orientation() { return m_roomRenderer->Orientation(); }
	REIGN_FORCEINLINING void Moving( _In_ boolean value ) { m_roomRenderer->Moving(value); }
	REIGN_FORCEINLINING boolean Moving() { return m_roomRenderer->Moving(); }

private:

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_blackBrush;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
	Windows::Foundation::Point m_textPosition;
	DWRITE_TEXT_METRICS m_textMetrics;
	int32 m_backgroundColorIndex;
	boolean m_renderNeeded;
	Reign::Vector4 m_clear;

	RoomRenderer^ m_roomRenderer;
	Room m_room;
};
