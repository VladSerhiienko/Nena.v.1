#pragma once

namespace Labwork1UI
{
	namespace Common
	{
		/// <summary>
		/// Value converter that translates true to <see cref="Visibility::Visible"/> and false
		/// to <see cref="Visibility::Collapsed"/>.
		/// </summary>
		public ref class BooleanToVisibilityConverter sealed : Windows::UI::Xaml::Data::IValueConverter
		{
		public:
			virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
			virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
		};
	}
}
