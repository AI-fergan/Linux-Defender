using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using Newtonsoft.Json;
using System.Collections.Generic;
using System;
using UIclient2.ViewModels;
using System.Threading.Tasks;
using Avalonia.Threading;

namespace UIclient2;

public class TimeData
{
    public int time { get; set; }
}
public partial class SettingsWindow : Window
{
    private int _time;
    public SettingsWindow() {
        Communication communication = new Communication();
        InitializeComponent();
        DataContext = new SettingsWindowViewModel(this);      

        communication.SendMSG(110, "");
        string respJson = communication.GetMSG();
        communication.close();
        _time = JsonConvert.DeserializeObject<Structs.GetTimeResponse>(respJson).time;
        FrequencyTextBox.Watermark += _time.ToString();

    }

    public void Submit_Click(object sender, RoutedEventArgs e)
    {
        Communication communication =  new Communication();
        string input = FrequencyTextBox.Text;
        if(string.IsNullOrEmpty(input) || !IsNumber(input))
        {
            ShowMessage("Input must be a number!", "Red");
        }
        else
        {
            try
            {
                TimeData data = new TimeData { time = int.Parse(input) };
                string json = JsonConvert.SerializeObject(data);
                communication.SendMSG(108, json);

                string respJson = communication.GetMSG();
                var respStruct = JsonConvert.DeserializeObject<Structs.ChangeScanTimeResponse>(respJson);
                if (respStruct.isWorked == 1)
                {
                    ShowMessage("Successfully changed scan frequency :)\nWill be active after the next scan.", "Green");
                    FrequencyTextBox.Watermark = "Current Frequency: " + input;
                    FrequencyTextBox.Text = "";
                }
            }
            catch (Exception ex)
            {
                ShowMessage("Couldn't change scan frequency :(\n" + ex.Message, "Red");
            }
        }
        communication.close();
    }

    private bool IsNumber(string s) {
        return double.TryParse(s, out _);
    }

    private void ShowMessage(string message, string color)
    {
        invalidTextBlock.IsVisible = true;
        invalidTextBlock.Text = message;
        invalidTextBlock.Foreground = color == "Red" ? Avalonia.Media.Brushes.Red : Avalonia.Media.Brushes.Green;

        var timer = new DispatcherTimer();
        timer.Interval = TimeSpan.FromSeconds(5);
        timer.Tick += (sender, e) =>
        {
            invalidTextBlock.IsVisible = false;
            timer.Stop();
        };
        timer.Start();
    }
}