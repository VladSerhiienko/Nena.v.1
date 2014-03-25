//
// SplitPage.xaml.h
// Declaration of the SplitPage class
//

#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "SplitPage.g.h"

namespace Labwork1UI
{
	/// <summary>
	/// A page that displays a group title, a list of items within the group, and details for the
	/// currently selected item.
	/// </summary>
	public ref class SplitPage sealed
	{
	public:
		SplitPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		virtual void GoBack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) override;
		virtual Platform::String^ DetermineVisualState(Windows::UI::ViewManagement::ApplicationViewState viewState) override;

	private:
		void ItemListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		bool UsingLogicalPageNavigation();
		bool UsingLogicalPageNavigation(Windows::UI::ViewManagement::ApplicationViewState viewState);
	};
}
