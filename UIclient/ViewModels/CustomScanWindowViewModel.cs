using Avalonia.Controls;
using ReactiveUI;
using System;
using System.Reactive;
using System.Threading.Tasks;
using System.Net;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using UIclient2.Views;
using Newtonsoft.Json;
using Avalonia.Threading;
using MsBox.Avalonia;
using MsBox.Avalonia.Dto;
using MsBox.Avalonia.Enums;
using MsBox.Avalonia.Models;
using System.IO;
using System.Linq;

namespace UIclient2.ViewModels
{
    public class YaraData
    {
        public string pathToScan { get; set; }
        public string scanType { get; set; }
        public string recursive { get; set; }
    }
    public class CustomScanWindowViewModel : ViewModelBase
    {
        private CustomScanWindow _customScanWindow;

        public ReactiveCommand<Unit, Unit> HomeCommand { get; }
        public ReactiveCommand<Unit, Unit> SettingsCommand { get; }
        public ReactiveCommand<Unit, Unit> ArchivesCommand { get; }
        public ReactiveCommand<Unit, Unit> ScanCommand { get; }

        public CustomScanWindowViewModel(CustomScanWindow customScanWindow)
        {
            _customScanWindow = customScanWindow;

            HomeCommand = ReactiveCommand.Create(() => {
                MainWindow mainWindow = new MainWindow();
                mainWindow.Show();
                _customScanWindow.Close();
            });

            SettingsCommand = ReactiveCommand.Create(() => {
                SettingsWindow settingsWindow = new SettingsWindow();
                settingsWindow.Show();
                _customScanWindow.Close();
            });

            ArchivesCommand = ReactiveCommand.Create(() => {
                ArchivesWindow archivesWindow = new ArchivesWindow();
                archivesWindow.Show();
                _customScanWindow.Close();
            });


            ScanCommand = ReactiveCommand.Create(ScanFiles);

        }


        public async void ScanFiles()
        {
            var dialog = new DialogView();
            var viewModel = new DialogViewModel();
            dialog.DataContext = viewModel;
            var window = new Window
            {
                Title = "Select File",
                Content = dialog,
                SizeToContent = SizeToContent.WidthAndHeight
            };
            viewModel.SelectionMade += async (sender, result) =>
            {
                if (result == "file")
                {
                    var openFileDialog = new OpenFileDialog
                    {
                        Title = "Select File",
                        Directory = ".",
                        AllowMultiple = false,
                    };
                    string[] filePaths = await openFileDialog.ShowAsync(window);
                    if (filePaths != null && filePaths.Length > 0)
                    {
                        Communication communication = new Communication();
                        string[][] results = communication.scan(Communication.scanTypes.file_scan, filePaths[0]);
                        
                        ResultsScanWindow resultsScanWindow = new ResultsScanWindow(results[0], results[1], results[2]);
                        resultsScanWindow.Show();
                    }
                }
                else
                {
                    var openFolderDialog = new OpenFolderDialog
                    {
                        Title = "Select Folder",
                        Directory = ".",
                    };
                    string folderPath = await openFolderDialog.ShowAsync(window);
                    if (!string.IsNullOrEmpty(folderPath))
                    {
                        Communication communication = new Communication();
                        string[][] results = await Task.Run(() => communication.scan(Communication.scanTypes.folder_scan, folderPath));
                        
                        ResultsScanWindow resultsScanWindow = new ResultsScanWindow(results[0], results[1], results[2]);
                        resultsScanWindow.Show();
                    }
                }
                window.Close();
            };

            window.ShowDialog(_customScanWindow);
        }
    }
}
