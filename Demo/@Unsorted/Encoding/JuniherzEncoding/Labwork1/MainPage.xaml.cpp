//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include <ppltasks.h>
#include "MainPage.xaml.h"

using namespace Labwork1;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Concurrency;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
	m_rw = ref new BasicReaderWriter();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter
}


void Labwork1::MainPage::OnResultButtonClicked1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_shafanoalphabet = ref new ShannonFanoAlphabet();
	m_huffalphabet = ref new HuffmanAlphabet();

	ResultTextBlock->Text = L"Thinking ...";

	m_rw->ReadTextDataAsync(L"labdata\\alphabet-mapping-03.txt", ref new ReadTextDataAsyncCallback([this](Platform::String^ text, AsyncStatus)
	{
		m_text = text;

		m_huffalphabet->Map(text);
		m_shafanoalphabet->Map(text);

		m_huffalphabet->GenerateEncoding();
		m_shafanoalphabet->GenerateEncoding();

		ResultTextBlock->Text = text;
	}));

	//m_rw->ReadTextDataAsync(L"labdata\\alphabet-01.txt", ref new ReadTextDataAsyncCallback([this](Platform::String^ text, AsyncStatus)
	//{
	//	m_text = text;

	//	m_shafanoalphabet->ReadFrom(text);
	//	//m_shafanoalphabet->GenerateEncoding();

	//	m_codec1 = ref new Codec(m_shafanoalphabet);

	//	ResultTextBlock->Text = text;
	//}));
}

void Labwork1::MainPage::OnResultButtonClicked2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_codec = ref new Codec(m_shafanoalphabet);
	ResultTextBlock->Text = m_shafanoalphabet->Save();
}


void Labwork1::MainPage::OnResultButtonClicked3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_codec = ref new Codec(m_huffalphabet);
	ResultTextBlock->Text = m_huffalphabet->Save();
}


void Labwork1::MainPage::OnResultButtonClicked4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ResultTextBlock->Text = m_codec->Encode(ResultTextBox1->Text);
}


void Labwork1::MainPage::OnResultButtonClicked5(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ResultTextBlock->Text = m_codec->Decode(ResultTextBox1->Text);
}
