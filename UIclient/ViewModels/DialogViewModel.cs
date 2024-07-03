using ReactiveUI;
using System;
using System.Reactive;

namespace UIclient2.ViewModels
{
    public class DialogViewModel : ViewModelBase
    {
        // Define events to notify the sender (MainWindowViewModel) of the result
        public event EventHandler<string> SelectionMade;

        // Example commands for selecting file or directory
        public ReactiveCommand<Unit, Unit> SelectFileCommand { get; }
        public ReactiveCommand<Unit, Unit> SelectDirectoryCommand { get; }

        public DialogViewModel()
        {
            // Initialize the commands
            SelectFileCommand = ReactiveCommand.Create(() => SelectionMade?.Invoke(this, "file"));
            SelectDirectoryCommand = ReactiveCommand.Create(() => SelectionMade?.Invoke(this, "directory"));
        }
    }
}
