using System.Windows.Controls;
using System.Windows.Input;

namespace EntityEditor.Controls
{
    /// <summary>
    /// Interaction logic for PrefabTile.xaml
    /// </summary>
    public partial class PrefabTile : UserControl
    {
        public PrefabTile()
        {
            InitializeComponent();
        }

        private void UIElement_OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            var data = DataContext as PrefabTileData;
            if (data == null)
                return;

            data.Randomize();
        }
    }
}
