using System.Collections.Generic;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using UIclient2.ViewModels;
using Newtonsoft.Json;
using System;
using System.Threading.Tasks;
using Avalonia.Threading;
using System.Security.Cryptography.X509Certificates;
using Avalonia.VisualTree;
using Avalonia.Controls.Templates;
using System.Linq.Expressions;

using Avalonia.Markup.Xaml;
using System.Net;
using System.Text.RegularExpressions;


namespace UIclient2.Views
{
    public partial class ArchivesWindow : Window
    {
        public ArchivesWindow()
        {
            InitializeComponent();
            DataContext = new ArchivesWindowViewModel(this);
        }

        public void block(object sender, RoutedEventArgs e){
            Structs.BlockRequest req;
            Structs.UpdateBlockedTableRequest update_req;
            if(proc_pid.Text.Length != 0) {
                req.pid = Int32.Parse(proc_pid.Text);
                string json = JsonConvert.SerializeObject(req);
                Communication communication = new Communication();
                communication.SendMSG(106, json);
                result(communication.GetMSG());

                update_req.action = "add";
                update_req.dateTime = "";
                update_req.pid = req.pid;
                json = JsonConvert.SerializeObject(update_req);
                communication.SendMSG(103, json);
            }
        }

        public void free(object sender, RoutedEventArgs e){
            Structs.FreeRequest req;
            Structs.UpdateBlockedTableRequest update_req;
            if(proc_pid.Text.Length != 0) {
                req.pid = Int32.Parse(proc_pid.Text);
                string json = JsonConvert.SerializeObject(req);
                Communication communication = new Communication();
                communication.SendMSG(105, json);
                result(communication.GetMSG());

                update_req.action = "remove";
                update_req.dateTime = "";
                update_req.pid = req.pid;
                json = JsonConvert.SerializeObject(update_req);
                communication.SendMSG(103, json);
            }
        }

        private void result(string json){
            Structs.FreeResponse result = JsonConvert.DeserializeObject<Structs.FreeResponse>(json);
            if(result.isWorked == 1){
                ResultsTextBlock.Text = "Operation ended successfully";
            }
            else{
                ResultsTextBlock.Text = "Operation failed";
            }

            ResultsTextBlock.IsVisible = true;
        }
    }

}
