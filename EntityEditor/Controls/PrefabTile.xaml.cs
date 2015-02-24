using System.Windows.Controls;
using System.Windows.Input;
using EntityEditor.Helpers;
using EntityEditor.Prefabs;

namespace EntityEditor.Controls
{
    /// <summary>
    ///     Interaction logic for PrefabTile.xaml
    /// </summary>
    public partial class PrefabTile : UserControl
    {
        public PrefabTile()
        {
            InitializeComponent();
            Loaded += delegate
            {
                if (StartWall)
                    DataContext = (PrefabTileData) Tiles.Prefabs[1];
            };
        }

        public bool StartWall { get; set; }

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

            var prevValue = (PrefabTileData) data.Clone();

            if (data.Base.Id == paintData.Base.Id)
                data.Reassign((PrefabTileData) Tiles.Prefabs[0]);
            else
                data.Reassign(paintData);

            var newValue = (PrefabTileData) data.Clone();

            EditHistory.PushUndo(new PrefabChangeHistory
            {
                Data = data,
                Prev = prevValue,
                Next = newValue
            }.Undo);
        }
    }
}