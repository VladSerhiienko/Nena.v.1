//
// BasicPage.xaml.h
// Declaration of the BasicPage class
//

#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "BasicPage.g.h"

namespace Labwork1UI
{
	/// <summary>
	/// A basic page that provides characteristics common to most applications.
	/// </summary>
	public ref class BasicPage sealed
	{
	public:
		BasicPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
	};
}
