using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Sockets;
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

namespace Lab4_Server
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        TcpListener server;
        public MainWindow()
        {
            InitializeComponent();
        }

        private async Task HandleClientAsync(TcpClient client)
        {
            var stream = client.GetStream();
            int Byte;
            var response = new List<byte>();
            string word;
            /*this.InfoBox.Text = "до цикла";
            while ((Byte = stream.ReadByte()) != -1)
            {
                response.Add((byte)Byte);
                word = Encoding.UTF8.GetString(response.ToArray());
            }

            this.InfoBox.Text = "после цикла";
            word = Encoding.UTF8.GetString(response.ToArray());*/
            while (true)
            {
                byte[] buffer = new byte[1024];
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                //Dispatcher.Invoke(() => InfoBox.Text = word);
                this.InfoBox.Text = message;
            }
        }

        private async void Window_Loaded(object sender, EventArgs e)
        {
            IPAddress localIp = IPAddress.Parse("127.0.0.1");
            server = new TcpListener(localIp, 8888);
            server.Start();
            //this.InfoBox.Text = "Server started";
            var client = await server.AcceptTcpClientAsync();
            //this.InfoBox.Text = client.Client.RemoteEndPoint.ToString();
            

        }

        private void Window_Closed(object sender, EventArgs e)
        {
            server.Stop();
        }
    }
}
