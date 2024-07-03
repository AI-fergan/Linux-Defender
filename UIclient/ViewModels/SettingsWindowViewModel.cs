using ReactiveUI;
using System;
using System.Reactive;
using System.Windows.Input;
using System.IO;
using Avalonia.Controls;
using UIclient2.Views;
using Newtonsoft.Json;


namespace UIclient2.ViewModels
{
    public class SettingsWindowViewModel : ViewModelBase
    {
        private SettingsWindow _settingsWindow;
        private string _scanFrequency;
        public string ScanFrequency
        {
            get => _scanFrequency;
            set => this.RaiseAndSetIfChanged(ref _scanFrequency, value);
        }

        private string _scanFrequencyInput;
        public string ScanFrequencyInput
        {
            get => _scanFrequencyInput;
            set => this.RaiseAndSetIfChanged(ref _scanFrequencyInput, value);
        }

        public ReactiveCommand<Unit, Unit> ArchivesCommand { get; }
        public ReactiveCommand<Unit, Unit> HomeCommand { get; }
        public ReactiveCommand<Unit, Unit> ChooseFolderCommand { get; }

        public SettingsWindowViewModel(SettingsWindow settingsWindow)
        {
            _settingsWindow = settingsWindow;

            ArchivesCommand = ReactiveCommand.Create(() => {
                ArchivesWindow archivesWindow = new ArchivesWindow();
                archivesWindow.Show();
                _settingsWindow.Close();
            });
            HomeCommand = ReactiveCommand.Create(() => {
                MainWindow mainWindow = new MainWindow();
                mainWindow.Show();
                _settingsWindow.Close();
            });
        }
    }
}
