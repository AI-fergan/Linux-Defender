using Avalonia.Controls;
using Avalonia.Interactivity;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using UIclient2.ViewModels;

namespace UIclient2.Views
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            DataContext = new MainWindowViewModel(this);
        }

        public async void QuickScan_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Communication communication = new Communication();
                string[][] results = await Task.Run(() => communication.scan(Communication.scanTypes.quick_scan, ""));
                
                ResultsScanWindow resultsScanWindow = new ResultsScanWindow(results[0], results[1], results[2]);
                resultsScanWindow.Show();
            }
            catch (Exception ex)
            {
                // Handle any exceptions
                Console.WriteLine($"Error: {ex.Message}");
            }
        }

        public async void FullScan_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string[] results = {""};// await Task.Run(() => scan("/home", "--scan-dir", "--recursive"));
                //ResultsScanWindow resultsScanWindow = new ResultsScanWindow(results);
                //resultsScanWindow.Show();
            }
            catch (Exception ex)
            {
                // Handle any exceptions
                Console.WriteLine($"Error: {ex.Message}");
            }
        }
    }
}
