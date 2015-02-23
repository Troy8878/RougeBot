using System;
using System.Collections.Generic;
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
using System.Windows.Shapes;

namespace EntityEditor.Controls
{
    /// <summary>
    /// Interaction logic for SetMetadata.xaml
    /// </summary>
    public partial class SetMetadata : Window
    {
        public static string GetString(string init = "")
        {
            var window = new SetMetadata(init);
            return window.ShowDialog() != true ? init : window.Input.Text;
        }

        private SetMetadata(string init = "")
        {
            InitializeComponent();
            Input.Text = init;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            Close();
        }
    }
}
