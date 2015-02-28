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
using System.Windows.Navigation;
using System.Windows.Shapes;
using EntityEditor.PrefabEditor.Controls;

namespace EntityEditor.Entities.Representations.Views
{
    /// <summary>
    /// Interaction logic for TexturePicker.xaml
    /// </summary>
    public partial class TexturePicker : UserControl
    {
        public TexturePicker()
        {
            InitializeComponent();
        }

        private void ClearTexture(object sender, RoutedEventArgs e)
        {
            Box.Text = "";
        }

        private void SelectClick(object sender, RoutedEventArgs e)
        {
            Popup.IsOpen = true;
        }

        private void ThumbnailOnMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            ImagePreview.IsOpen = true;
        }

        private void OnOverride(object sender, RoutedEventArgs e)
        {
            Box.Text = SetMetadata.GetString(Box.Text);
        }
    }
}
