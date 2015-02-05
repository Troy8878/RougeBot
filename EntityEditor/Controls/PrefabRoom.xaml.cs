using System.Windows.Controls;

namespace EntityEditor.Controls
{
    /// <summary>
    /// Interaction logic for PrefabRoom.xaml
    /// </summary>
    public partial class PrefabRoom
    {
        public PrefabRoom()
        {
            InitializeComponent();

            Randomize();
        }

        public void Randomize()
        {
            var data = new PrefabTileData[10][];
            for (var i = 0; i < 10; ++i)
            {
                data[i] = new PrefabTileData[10];
                for (var j = 0; j < 10; ++j)
                {
                    data[i][j] = new PrefabTileData();
                }
            }
            Items.ItemsSource = data;
        }
    }
}
