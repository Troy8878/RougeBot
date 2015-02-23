using System;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using EntityEditor.Helpers;
using EntityEditor.Prefabs;

namespace EntityEditor.Controls
{
    /// <summary>
    /// Interaction logic for PrefabTile.xaml
    /// </summary>
    public partial class PrefabTile : UserControl
    {
        public bool StartWall { get; set; }

        public PrefabTile()
        {
            InitializeComponent();
            Loaded += delegate
            {
                if (StartWall)
                    DataContext = (PrefabTileData) Tiles.Prefabs[1];
            };
        }

        private void UIElement_OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (StartWall)
                return;

            var data = DataContext as PrefabTileData;
            if (data == null)
                return;

            var paintData = this.GetParent<PrefabRoom>().PaintTile.DataContext as PrefabTileData;
            if (paintData == null)
                return;

            if (data.Base.Id == paintData.Base.Id)
                DataContext = (PrefabTileData) Tiles.Prefabs[0];
            else
                DataContext = paintData.Clone();
        }
    }
}
