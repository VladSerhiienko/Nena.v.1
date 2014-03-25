//
// ItemsPage.xaml.h
// Declaration of the ItemsPage class
//

#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "ItemsPage.g.h"

namespace Labwork1UI
{
	/// <summary>
	/// A page that displays a collection of item previews.  In the Split Application this page
	/// is used to display and select one of the available groups.
	/// </summary>
	public ref class ItemsPage sealed
	{
	public:
		ItemsPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;

	private:
		void ItemView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
