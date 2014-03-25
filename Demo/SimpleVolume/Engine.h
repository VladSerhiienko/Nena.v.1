#pragma once

#include <Nena/App.h>
#include <Nena/AppVars.h>
#include <Nena/PlatformAssist.h>
#include <Nena/DeviceResources.h>
#include <Nena/RenderTargetOverlay.h>

#ifndef __RASTERTEK_ENGINE_INCLUDED__
#define __RASTERTEK_ENGINE_INCLUDED__

namespace Demo
{
	struct Input;
	struct Graphics;
	struct Engine;

	struct EngineNotify : Nena::Graphics::IDeviceNotify
	{
		virtual void OnDeviceLost() override;
		virtual void OnDeviceRestored() override;
	};

	struct Input
	{
		Nena::Dispatcher::KeyboardEvent KeyPressed;
		Nena::Dispatcher::KeyboardEvent KeyReleased;

		Nena::Dispatcher::MouseEvent MouseMoved;
		Nena::Dispatcher::MouseEvent MouseLeftButtonPressed;
		Nena::Dispatcher::MouseEvent MouseLeftButtonReleased;
		Nena::Dispatcher::MouseEvent MouseRightButtonPressed;
		Nena::Dispatcher::MouseEvent MouseRightButtonReleased;

		Input();

		static LRESULT _Nena_DispatcherCallTy_ KeyboardDispatchCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);
		static LRESULT _Nena_DispatcherCallTy_ MouseDispatchCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);

	};

	struct Graphics
	{
		Nena::Graphics::DeviceResources D3D;
		Nena::Graphics::OverlayResources D2D;

		Graphics();
		~Graphics();
	};

	struct Engine : public Graphics, public Input
	{
		Demo::EngineNotify Notify;

		void OnDeviceLost();
		void OnDeviceRestored();
		static __forceinline Engine  *__fastcall GetForCurrentThread()
		{
			static Engine engine; return &engine;
		}

	private:

		Engine();
		static void _Nena_AppCallbackObjCallTy_ OnUpdate(_In_ Nena::App *app);

	};
}

#endif // !__RASTERTEK_ENGINE_INCLUDED__
