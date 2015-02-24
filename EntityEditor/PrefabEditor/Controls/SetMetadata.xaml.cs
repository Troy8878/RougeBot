using System.Windows;

namespace EntityEditor.PrefabEditor.Controls
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
