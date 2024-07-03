using Avalonia.Controls;
using UIclient2.ViewModels;

namespace UIclient2;

public partial class ResultsScanWindow : Window
{
    public ResultsScanWindow(string[] FilesResults, string[] TablesResults, string[] NetworkResults)
    {
        InitializeComponent();
        DataContext = new ResultsWindowViewModel(FilesResults, TablesResults, NetworkResults);
    }
}