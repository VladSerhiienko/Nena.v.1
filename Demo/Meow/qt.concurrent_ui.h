#pragma once

#include "app.precompiled.h"

namespace Nena
{
	template<typename TResultAsync, typename TFunction>
	void QtCoWait(concurrency::task<void> task, TFunction func)
	{
		HANDLE handle = ::CreateEventA(
			NULL, FALSE, FALSE,
			"task_event"
			);

		task.continue_with([handle]()
		{
			::SetEvent(handle);
		});

		MSG msg;
		while (true)
		{
			DWORD dwResult = ::MsgWaitForMultipleObjects(1,
				&handle, FALSE, INFINITE, QS_ALLEVENTS
				);

			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				QApplication::processEvents();
			}
			else
			{
				::CloseHandle(handle);
				func();
				break;
			}
		}
	}

	template<typename TResultAsync, typename TFunction>
	void QtCoWait2(concurrency::task<TResultAsync> task, TFunction func)
	{
		HANDLE handle = ::CreateEventA(
			NULL, FALSE, FALSE,
			"task_event"
			);

		TResultAsync result;
		task.then([&result, handle](TResultAsync t)
		{
			result = t;
			::SetEvent(handle);
		});

		while (true)
		{
			DWORD dwResult = ::MsgWaitForMultipleObjects(1,
				&handle, FALSE, INFINITE, QS_TIMER | QS_PAINT
				);

			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				QApplication::processEvents();
			}
			else
			{
				::CloseHandle(handle);
				func(result);
				break;
			}
		}
	}

}
