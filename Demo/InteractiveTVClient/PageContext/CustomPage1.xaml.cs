using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace InteractiveTV.PageContext
{
    /// <summary>
    /// Interaction logic for CustomPage1.xaml
    /// </summary>
    public partial class CustomPage1 : UserControl
    {
        public CustomPage1()
        {
            Loaded += UserControl_Loaded;
            InitializeComponent();
            Loaded += UserControl_Loaded;
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("UserControl_Loaded");
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            Debug.WriteLine("UserControl_IsVisibleChanged");
        }
    }
}
