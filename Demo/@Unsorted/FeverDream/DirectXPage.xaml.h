//
// BlankPage.xaml.h
// Declaration of the BlankPage.xaml class.
//

#pragma once

#include "pch.h"
#include "DirectXPage.g.h"
#include "GameRenderer.h"
#include "BasicTimer.h"

namespace FeverDream
{
	/// <summary>
	/// A DirectX page that can be used on its own.  Note that it may not be used within a Frame.
	/// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
	public ref class DirectXPage sealed
	{
#pragma region Timer
		typedef Application::Utility::Timing::TimerView TimerView;
		typedef Application::Utility::Timing::Timer Timer;
#pragma endregion
	public:
		DirectXPage();

		void OnPreviousColorPressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnNextColorPressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
		void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);

	private:
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
		void OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
		void OpenConnectionButton_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CloseConnectionButton_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnLogicalDpiChanged(Platform::Object^ sender);
		void OnOrientationChanged(Platform::Object^ sender);
		void OnDisplayContentsInvalidated(Platform::Object^ sender);
		void OnRendering(Object^ sender, Object^ args);

		void OnConnectionChanged(
			_In_ ClientContext^ sender, 
			_In_ boolean connected
			)
		{
			m_connected = connected;

			if (m_connected) PrintToDebugWindowOnly8( "Online" )
			else PrintToDebugWindowOnly8( "Offline" )

			m_renderer->OnConnectionChanged(
				connected
				);
		}

		Windows::Foundation::EventRegistrationToken m_eventToken;

		GameRenderer^ m_renderer;
		bool m_renderNeeded;
		bool m_connected;

		Windows::Foundation::Point m_lastPoint;
		bool m_lastPointValid;
		
		//BasicTimer^ m_timer;
		DirectXPage::Timer m_timer; 
	};
}
