//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "encoding\foundation\basicrwasync.h"
#include "encoding\shannon-fano\shafalphabet.h"
#include "encoding\huffman\huffalphabet.h"
#include "encoding\foundation\basiccodec.h"

namespace Labwork1
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		//void OnResultButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		//void ResultButton1_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnResultButtonClicked1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnResultButtonClicked2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnResultButtonClicked3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnResultButtonClicked4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnResultButtonClicked5(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	private:
		BasicReaderWriter^	m_rw;
		Platform::String^	m_text;

		ShannonFanoAlphabet^		m_shafanoalphabet;
		HuffmanAlphabet^			m_huffalphabet;
		Codec^						m_codec;
	};
}
