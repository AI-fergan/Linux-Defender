using ReactiveUI;
using System.Collections.ObjectModel;

namespace UIclient2.ViewModels
{
    public class ResultsWindowViewModel : ViewModelBase
    {
        private ObservableCollection<string> _filesResults;
        private ObservableCollection<string> _tablesResults;
        private ObservableCollection<string> _networkResults;
        public ObservableCollection<string> FilesResults
        {
            get => _filesResults;
            private set => this.RaiseAndSetIfChanged(ref _filesResults, value);
        }
        public ObservableCollection<string> TablesResults
        {
            get => _tablesResults;
            private set => this.RaiseAndSetIfChanged(ref _tablesResults, value);
        }
        public ObservableCollection<string> NetworkResults
        {
            get => _networkResults;
            private set => this.RaiseAndSetIfChanged(ref _networkResults, value);
        }

        public ResultsWindowViewModel(string[] filesResults, string[] tablesResults, string[] networkResults)
        {
            FilesResults = new ObservableCollection<string>(filesResults);
            TablesResults = new ObservableCollection<string>(tablesResults);
            NetworkResults = new ObservableCollection<string>(networkResults);
        }
    }
}

