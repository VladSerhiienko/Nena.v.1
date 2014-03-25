//
// ItemsPage.xaml.cpp
// Implementation of the ItemsPage class
//

#include "pch.h"
#include "DataModel/SampleDataSource.h"
#include "ItemsPage.xaml.h"
#include "SplitPage.xaml.h"

using namespace Labwork1UI;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Items Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234233

ItemsPage::ItemsPage()
{
	InitializeComponent();
}

/// <summary>
/// Populates the page with content passed during navigation.  Any saved state is also
/// provided when recreating a page from a prior session.
/// </summary>
/// <param name="navigationParameter">The parameter value passed to
/// <see cref="Frame::Navigate(Type, Object)"/> when this page was initially requested.
/// </param>
/// <param name="pageState">A map of state preserved by this page during an earlier
/// session.  This will be null the first time a page is visited.</param>
void ItemsPage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
	(void) pageState;	// Unused parameter

	// TODO: Create an appropriate data model for your problem domain to replace the sample data
	auto sampleDataGroups = Data::SampleDataSource::GetGroups(safe_cast<String^>(navigationParameter));
	DefaultViewModel->Insert("Items", sampleDataGroups);
}

/// <summary>
/// Invoked when an item is clicked.
/// </summary>
/// <param name="sender">The GridView (or ListView when the application is snapped)
/// displaying the item clicked.</param>
/// <param name="e">Event data that describes the item clicked.</param>
void ItemsPage::ItemView_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
	(void) sender;	// Unused parameter

	// Navigate to the appropriate destination page, configuring the new page
	// by passing required information as a navigation parameter
	auto groupId = safe_cast<Data::SampleDataGroup^>(e->ClickedItem)->UniqueId;
	Frame->Navigate(TypeName(SplitPage::typeid), groupId);
}
