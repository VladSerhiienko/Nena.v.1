using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CSWPFApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Physics.ProjectileSimulatorConfig Config;
        Physics.ProjectileSimulator Simulator;

        public MainWindow()
        {
            InitializeComponent();

            Config = new Physics.ProjectileSimulatorConfig();
            Simulator = new Physics.ProjectileSimulator();
        }

        private void OnConfigurationLoaded(object sender, RoutedEventArgs e)
        {
            SimulatorConfigDataGrid.ItemsSource = Config.Items;
        }

        private void OnSimulateButtonClicked(object sender, RoutedEventArgs e)
        {
            Simulator.ReadParameters(Config.Items);

            Polyline topView, sideView;
            Simulator.GenerateTrajectory(out topView, out sideView);

            sideView.Stroke = Brushes.DodgerBlue;
            topView.Stroke = Brushes.DodgerBlue;

            SideViewCanvas.Children.Add(sideView);
            TopViewCanvas.Children.Add(topView);
        }

        private void OnWindowResized(object sender, SizeChangedEventArgs e)
        {
            Simulator.Cnv.L = (float)TopViewCanvas.ActualWidth;
            Simulator.Cnv.W = (float)TopViewCanvas.ActualHeight;
            Simulator.Cnv.H = (float)SideViewCanvas.ActualHeight;

            Simulator.OnWindowResized();

            //System.Diagnostics.Debug.WriteLine(
            //    "Canvas => L=" + Simulator.Cnv.L + " " + 
            //    "W=" + Simulator.Cnv.W + " " + 
            //    "H=" + Simulator.Cnv.H
            //    );
        }

        private void OnClearButtonClicked(object sender, RoutedEventArgs e)
        {
            //SideViewCanvas.Children.Clear();
            //TopViewCanvas.Children.Clear();
        }
    }
}
