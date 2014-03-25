#pragma once

#include <Nena\Window.h>
#include <Nena\StepTimer.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Skin.h"
#include "Camera.h"
#include "ContentLoader.h"
#include "ShapeRenderer.h"

namespace Framework
{
	namespace Application
	{
		void Start();
		void Stop();

		struct Window;
		struct Dispatcher
		{
			Application::Window *Host;

			Dispatcher(
				_In_ Application::Window *host
				);
			LRESULT Process(
				_In_ ::HWND hwnd,
				_In_ ::UINT umsg,
				_In_ ::WPARAM wparam,
				_In_ ::LPARAM lparam
				);
		};

		struct WindowDeviceNotify : Nena::Graphics::IDeviceNotify
		{
			Application::Window *Host;
			WindowDeviceNotify(
				_In_ Application::Window *host
				);

			virtual void OnDeviceLost() override;
			virtual void OnDeviceRestored() override;
		};

		struct Window : Nena::Application::Window
		{
			friend WindowDeviceNotify;

			Window::Window();
			::BOOL Update();

			LRESULT OnMouseLeftButtonPressed(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT OnMouseRightButtonPressed(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT OnMouseLeftButtonReleased(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT OnMouseRightButtonReleased(_In_::INT16 x, _In_ ::INT16 y);
			LRESULT OnMouseMoved(_In_ ::INT16 x, _In_ ::INT16 y);
			LRESULT OnKeyPressed(_In_ ::UINT16 virtualKey);
			LRESULT OnMouseWheel(_In_ ::INT32 delta);
			LRESULT OnResized();

			static Application::Window *GetForCurrentThread();

			::Nena::Graphics::OverlayResources Overlay;
			::Nena::Graphics::DeviceResources Graphics;
			::Nena::Simulation::StepTimer Timer;

			Demo::ShapeRenderer Renderer;
			Demo::ContentLoader Loader;
			Demo::Skin Content;
			Demo::Camera Cam;

			Application::WindowDeviceNotify DeviceNotify;
			Application::Dispatcher EventHandler;
			::BOOL RMButtonPressed = FALSE;
		};
	}
}