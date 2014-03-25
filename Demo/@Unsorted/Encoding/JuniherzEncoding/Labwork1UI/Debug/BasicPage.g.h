﻿

#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------

namespace Labwork1UI {
    namespace Common {
        ref class LayoutAwarePage;
    }
}
namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class Button;
                ref class TextBox;
                ref class ListBox;
                ref class TextBlock;
            }
        }
    }
}
namespace Windows {
    namespace UI {
        namespace Xaml {
            ref class VisualStateGroup;
            ref class VisualState;
        }
    }
}

namespace Labwork1UI
{
    partial ref class BasicPage : public Labwork1UI::Common::LayoutAwarePage, 
        public Windows::UI::Xaml::Markup::IComponentConnector
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, Platform::Object^ target);
    
    private:
        bool _contentLoaded;
    
        private: Labwork1UI::Common::LayoutAwarePage^ pageRoot;
        private: Windows::UI::Xaml::Controls::Button^ HuffmanEncodeButton;
        private: Windows::UI::Xaml::Controls::Button^ HuffmanGenerateButton;
        private: Windows::UI::Xaml::Controls::Button^ HuffmanDecodeButton;
        private: Windows::UI::Xaml::Controls::TextBox^ HuffmanEncodeText;
        private: Windows::UI::Xaml::Controls::TextBox^ HuffmanDecodeText;
        private: Windows::UI::Xaml::Controls::ListBox^ HuffmanAlphabetListBox;
        private: Windows::UI::Xaml::Controls::TextBox^ HuffmanReadTextBox;
        private: Windows::UI::Xaml::Controls::Button^ HuffmanReadButton;
        private: Windows::UI::Xaml::Controls::TextBox^ HuffmanMapTextBox;
        private: Windows::UI::Xaml::Controls::Button^ HuffmanMapButton;
        private: Windows::UI::Xaml::Controls::Button^ ShannonFanoEncodeButton;
        private: Windows::UI::Xaml::Controls::Button^ ShannonFanoGenerateButton;
        private: Windows::UI::Xaml::Controls::Button^ ShannonFanoDecodeButton;
        private: Windows::UI::Xaml::Controls::TextBox^ ShannonFanoEncodeText;
        private: Windows::UI::Xaml::Controls::TextBox^ ShannonFanoDecodeText;
        private: Windows::UI::Xaml::Controls::ListBox^ ShannonFanoAlphabetListBox;
        private: Windows::UI::Xaml::Controls::TextBox^ ShannonFanoReadTextBox;
        private: Windows::UI::Xaml::Controls::Button^ ShannonFanoReadButton;
        private: Windows::UI::Xaml::Controls::TextBox^ ShannonFanoMapTextBox;
        private: Windows::UI::Xaml::Controls::Button^ ShannonFanoMapButton;
        private: Windows::UI::Xaml::Controls::TextBlock^ pageTitle;
        private: Windows::UI::Xaml::VisualStateGroup^ ApplicationViewStates;
        private: Windows::UI::Xaml::VisualState^ FullScreenLandscape;
        private: Windows::UI::Xaml::VisualState^ Filled;
        private: Windows::UI::Xaml::VisualState^ FullScreenPortrait;
        private: Windows::UI::Xaml::VisualState^ Snapped;
    };
}

