using Avalonia.Controls;
using ReactiveUI;
using System.Reactive;
using System.Threading.Tasks;
using UIclient2.Views;

namespace UIclient2.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        private MainWindow _mainWindow;
        // private Communication _communication;

        public ReactiveCommand<Unit, Unit> ArchivesCommand { get; }
        public ReactiveCommand<Unit, Unit> SettingsCommand { get; }
        public ReactiveCommand<Unit, Unit> CustomScanCommand { get; }

        public MainWindowViewModel(MainWindow mainWindow)
        {
            _mainWindow = mainWindow;
            // _communication = new Communication(); // If Communication class exists

            ArchivesCommand = ReactiveCommand.Create(() =>
            {
                var archivesWindow = new ArchivesWindow();
                archivesWindow.Show();
                _mainWindow.Close();
            });

            SettingsCommand = ReactiveCommand.Create(() =>
            {
                var settingsWindow = new SettingsWindow();
                settingsWindow.Show();
                _mainWindow.Close();
            });

            CustomScanCommand = ReactiveCommand.Create(() => {
                CustomScanWindow customScanWindow = new CustomScanWindow();
                customScanWindow.Show();
                _mainWindow.Close();
            });
        }
           
    }
}
