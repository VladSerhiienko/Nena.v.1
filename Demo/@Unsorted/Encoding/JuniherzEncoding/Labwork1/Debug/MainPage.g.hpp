﻿

//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"
#include "MainPage.xaml.h"

using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Markup;
using namespace Labwork1;

void MainPage::InitializeComponent()
{
    if (_contentLoaded)
        return;

    _contentLoaded = true;

    // Call LoadComponent on ms-appx:///MainPage.xaml
    Windows::UI::Xaml::Application::LoadComponent(this, ref new Windows::Foundation::Uri(L"ms-appx:///MainPage.xaml"), Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);

    // Get the TextBlock named 'ResultTextBlock'
    ResultTextBlock = safe_cast<Windows::UI::Xaml::Controls::TextBlock^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultTextBlock"));
    // Get the Button named 'ResultButton1'
    ResultButton1 = safe_cast<Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultButton1"));
    // Get the Button named 'ResultButton2'
    ResultButton2 = safe_cast<Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultButton2"));
    // Get the Button named 'ResultButton3'
    ResultButton3 = safe_cast<Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultButton3"));
    // Get the Button named 'ResultButton4'
    ResultButton4 = safe_cast<Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultButton4"));
    // Get the TextBox named 'ResultTextBox1'
    ResultTextBox1 = safe_cast<Windows::UI::Xaml::Controls::TextBox^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultTextBox1"));
    // Get the Button named 'ResultButton5'
    ResultButton5 = safe_cast<Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResultButton5"));
}

void MainPage::Connect(int connectionId, Platform::Object^ target)
{
    switch (connectionId)
    {
    case 1:
        (safe_cast<Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new Windows::UI::Xaml::RoutedEventHandler(this, (void (MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::OnResultButtonClicked1);
        break;
    case 2:
        (safe_cast<Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new Windows::UI::Xaml::RoutedEventHandler(this, (void (MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::OnResultButtonClicked2);
        break;
    case 3:
        (safe_cast<Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new Windows::UI::Xaml::RoutedEventHandler(this, (void (MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::OnResultButtonClicked3);
        break;
    case 4:
        (safe_cast<Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new Windows::UI::Xaml::RoutedEventHandler(this, (void (MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::OnResultButtonClicked4);
        break;
    case 5:
        (safe_cast<Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new Windows::UI::Xaml::RoutedEventHandler(this, (void (MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::OnResultButtonClicked5);
        break;
    }
    (void)connectionId; // Unused parameter
    (void)target; // Unused parameter
    _contentLoaded = true;
}
