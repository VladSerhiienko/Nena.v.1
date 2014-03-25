#include <Nena/Window.h>

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

		struct Window : Nena::Application::Window
		{
			LRESULT Framework::Application::Window::OnMouseMoved(
				_In_::INT16 x, _In_::INT16 y
				);
			LRESULT Framework::Application::Window::OnMouseLeftButtonPressed(
				_In_::INT16 x, _In_::INT16 y
				);
			LRESULT Framework::Application::Window::OnMouseRightButtonPressed(
				_In_ ::INT16 x, _In_ ::INT16 y 
				);
			LRESULT Framework::Application::Window::OnMouseLeftButtonReleased(
				_In_ ::INT16 x, _In_ ::INT16 y 
				);
			LRESULT Framework::Application::Window::OnMouseRightButtonReleased(
				_In_::INT16 x, _In_::INT16 y
				);

			static Framework::Application::Window *Framework::Application::Window::GetForCurrentThread(
				);
			Framework::Application::Window::Window(
				);

			inline void Frame(
				);

			Dispatcher EventHandler;
		};
	}
}