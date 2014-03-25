#pragma once

#include <Nena\Window.h>
#include <Nena\StepTimer.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "TextRenderer.h"

namespace Framework
{
	namespace Application
	{
		void Start();
		void Stop();

		struct Window;
		struct Dispatcher
		{
			Framework::Application::Window *Host;

			Framework::Application::Dispatcher::Dispatcher(
				Framework::Application::Window *host
				);
			LRESULT Framework::Application::Dispatcher::Process(
				_In_::HWND hwnd,
				_In_::UINT umsg,
				_In_::WPARAM wparam,
				_In_::LPARAM lparam
				);
		};

		struct WindowDeviceNotify : Nena::Graphics::IDeviceNotify
		{
			Framework::Application::Window *Host;
			Framework::Application::WindowDeviceNotify::WindowDeviceNotify(Framework::Application::Window *host);

			virtual void OnDeviceLost() override;
			virtual void OnDeviceRestored() override;
		};

		struct Window : Nena::Application::Window
		{
			friend WindowDeviceNotify;

			Framework::Application::Window::Window();
			::BOOL Framework::Application::Window::Update();

			LRESULT Framework::Application::Window::OnMouseLeftButtonPressed(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT Framework::Application::Window::OnMouseRightButtonPressed(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT Framework::Application::Window::OnMouseLeftButtonReleased(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT Framework::Application::Window::OnMouseRightButtonReleased(_In_::INT16 x, _In_::INT16 y);
			LRESULT Framework::Application::Window::OnMouseMoved(_In_::INT16 x, _In_::INT16 y);
			LRESULT Framework::Application::Window::OnKeyPressed(::UINT16 virtualKey);
			LRESULT Framework::Application::Window::OnResized();

			static Framework::Application::Window *Framework::Application::Window::GetForCurrentThread();

			Nena::Graphics::OverlayResources Overlay;
			Nena::Graphics::DeviceResources Graphics;
			Nena::Simulation::StepTimer Timer;

			TextRenderer Renderer;

			WindowDeviceNotify DeviceNotify;
			Dispatcher EventHandler;
		};
	}
}