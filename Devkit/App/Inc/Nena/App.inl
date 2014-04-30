
__forceinline Nena::App *Nena::App::GetForCurrentThread()
{
	static App app; return &app;
}

__forceinline LRESULT CALLBACK Nena::App::Loop(
	_In_ HWND hwnd, _In_ UINT msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static App *app = App::GetForCurrentThread();
	return app->Handler.Dispatch(hwnd, msg, wparam, lparam);
}

__forceinline LRESULT _Nena_DispatcherCallTy_ Nena::Dispatcher::CaughtStub::Null(
	_In_ Event *e, _In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	return 0;
}

__forceinline LRESULT _Nena_DispatcherCallTy_ Nena::Dispatcher::Stub::Null(
	_In_ Event *e, _In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	return DefWindowProc(
		hwnd, msg, wparam, lparam
		);
}

__forceinline LRESULT _Nena_DispatcherCallTy_ Nena::Dispatcher::Dispatch(
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	/*if (Root)*/
	return Root->Dispatch(
		hwnd, msg, wparam, lparam
		);
	/*else return DefWindowProc(
	hwnd, msg, wparam, lparam
	);*/
}

template <typename _TyCallback>
__forceinline void Nena::App::CoWait(
	_In_ concurrency::task<void> task,
	_In_ _TyCallback callback,
	_In_opt_ BOOL timer,
	_In_opt_ LPCSTR name
	)
{
	auto handle = ::CreateEventA(
		NULL, FALSE, FALSE, name
		);

	task.continue_with([handle]()
	{
		::SetEvent(handle);
	});

	::MSG e;
	auto app = Nena::App::GetForCurrentThread();
	if (timer) while(true)
	{
		::DWORD dwResult = ::MsgWaitForMultipleObjects(1,
			&handle, FALSE, INFINITE, QS_ALLEVENTS
			);

		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			if (::PeekMessageA(&e, wnd, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&e);
				::DispatchMessageA(&e);
			}

			if (e.message != WM_QUIT) { app->OnUpdate(this); continue; }
			else app->OnUpdate = nullptr;
		}
		else
		{
			::CloseHandle(handle);
			callback();
			break;
		}
	}
	else while(true)
	{
		DWORD dwResult = ::MsgWaitForMultipleObjects(1,
			&handle, FALSE, INFINITE, QS_ALLEVENTS
			);

		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			if (::PeekMessageA(&e, wnd, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&e);
				::DispatchMessageA(&e);
			}

			if (e.message != WM_QUIT)
			{
				app->Timer.Tick([app]() { app->OnUpdate(this); });
				continue;
			}
			else app->OnUpdate = nullptr;
		}
		else
		{
			::CloseHandle(handle);
			callback();
			break;
		}
	}

}

__forceinline Nena::Simulation::StepTimer const *Nena::App::GetTimer() const
{
	return &Timer;
}

__forceinline Nena::Simulation::StepTimer *Nena::App::GetTimer()
{
	return &Timer;
}