using ReactiveUI;
using System.Collections.Generic;
using System.Reactive;
using UIclient2.Views;
using Newtonsoft.Json;
using System.Collections.ObjectModel;
using System.IO;
using System;
using DynamicData;

namespace UIclient2.ViewModels
{
    public class ArchivesWindowViewModel : ViewModelBase
    {
        public ReactiveCommand<Unit, Unit> HomeCommand { get; }
        public ReactiveCommand<Unit, Unit> SettingsCommand { get; }
        public ReactiveCommand<Unit, Unit> VirusScanCommand { get; }
        public ReactiveCommand<Unit, Unit> BlockedCommand { get; }
        private bool _isTable1Visible;
        public bool IsTable1Visible
        {
            get => _isTable1Visible;
            set => this.RaiseAndSetIfChanged(ref _isTable1Visible, value);
        }

        private bool _isTable2Visible;
        public bool IsTable2Visible
        {
            get => _isTable2Visible;
            set => this.RaiseAndSetIfChanged(ref _isTable2Visible, value);
        }
        public ObservableCollection<string> DataForTable1 { get; private set; }
        public ObservableCollection<KeyValuePair<string, string>> DataForTable2 { get; private set; }
        public ArchivesWindowViewModel(ArchivesWindow archivesWindow)
        {
            IsTable1Visible = false; 
            IsTable2Visible = false;

            // Initialize the data sources for both tables
            DataForTable1 = new ObservableCollection<string>();
            DataForTable2 = new ObservableCollection<KeyValuePair<string, string>>();

            // Initialize commands
            VirusScanCommand = ReactiveCommand.Create(ToggleTable1);
            BlockedCommand = ReactiveCommand.Create(ToggleTable2);

            HomeCommand = ReactiveCommand.Create(() => {
                MainWindow mainWindow = new MainWindow();
                mainWindow.Show();
                archivesWindow.Close();
            });

            SettingsCommand = ReactiveCommand.Create(() => {
                SettingsWindow settingsWindow = new SettingsWindow();
                settingsWindow.Show();
                archivesWindow.Close();
            });

        }
        private void ToggleTable1()
        {
            Communication communication = new Communication();
            IsTable1Visible = !IsTable1Visible;
            if (IsTable1Visible)
            {
                // Populate table 1 with relevant data
                DataForTable1.Clear();
                communication.SendMSG(101, "");
                string respJson = communication.GetMSG();
                var respStruct = JsonConvert.DeserializeObject<Structs.SuspiciousListResponse>(respJson);
               foreach (string procName in respStruct.procNames)
                {
                    if (int.TryParse(procName, out int pid))
                    {
                        string processName = GetNameByPid(procName);
                        if (!string.IsNullOrEmpty(processName))
                            DataForTable1.Add(processName);
                    }
                    else
                        DataForTable1.Add(procName);
                }
            }

            communication.close();
        }

        private void ToggleTable2()
        {
            Communication communication = new Communication();
            IsTable2Visible = !IsTable2Visible;
            if (IsTable2Visible)
            {
                // Populate table 2 with relevant data
                DataForTable2.Clear();
                communication.SendMSG(102, "");
                string respJson = communication.GetMSG();
                var respStruct = JsonConvert.DeserializeObject<Structs.SuspendedListResponse>(respJson);
                foreach (string pid in respStruct.procPids)
                {
                    string procName = GetNameByPid(pid);
                    if(!string.IsNullOrEmpty(procName))
                        DataForTable2.Add(new KeyValuePair<string, string>(procName, pid));
                }               
            }
            communication.close();
        }

        private string GetNameByPid(string pid)
        {
            try
            {
                string procPath = @$"/proc/{pid}/comm";
                if (File.Exists(procPath))
                    return File.ReadAllText(procPath).Trim();
                return null;
            }
            catch (Exception ex){
                return null;
            }
        }
    }
}
