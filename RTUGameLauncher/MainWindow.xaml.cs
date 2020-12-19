//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game.
//
//RTU Game is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with RTU Game.If not, see < https://www.gnu.org/licenses/>.
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace RTUGameLauncher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        public Process gameProcess;
        public Process serverProcess;
        private void StartLocalGameButton_Click(object sender, RoutedEventArgs e)
        {
            if (serverProcess == null || serverProcess.HasExited)
            {
                ProcessStartInfo startinfo = new ProcessStartInfo();
                startinfo.FileName = "RTUGameServer/RTUGameServer";
                startinfo.WorkingDirectory = "RTUGameServer";
                serverProcess = Process.Start(startinfo);
            }
            if (gameProcess == null || gameProcess.HasExited)
            {
                ProcessStartInfo startinfo2 = new ProcessStartInfo();
                startinfo2.ArgumentList.Add("-connect");
                startinfo2.ArgumentList.Add("localhost");
                startinfo2.ArgumentList.Add("25565");
                startinfo2.ArgumentList.Add((_selectGraphicsAPI.SelectedItem as ComboBoxItem).Tag.ToString());
                startinfo2.FileName = "RTUGame/RTUGame";
                startinfo2.WorkingDirectory = "RTUGame";
                gameProcess = Process.Start(startinfo2);
            }

        }

        private void StartServerButton_Click(object sender, RoutedEventArgs e)
        {
            if (serverProcess == null || serverProcess.HasExited)
            {
                ProcessStartInfo startinfo = new ProcessStartInfo();
                startinfo.FileName = "RTUGameServer/RTUGameServer";
                startinfo.WorkingDirectory = "RTUGameServer";
                serverProcess = Process.Start(startinfo);
            }
            else
            {
                MessageBox.Show("The server was running.","Warning");
            }
        }
    }
}
