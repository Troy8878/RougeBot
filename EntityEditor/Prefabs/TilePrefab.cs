using System.Windows.Media;

namespace EntityEditor.Prefabs
{
    public class TilePrefab
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public Color EditColor { get; set; }

        public static explicit operator PrefabTileData(TilePrefab prefab)
        {
            return new PrefabTileData
            {
                Name = prefab.Name,
                Color = new SolidColorBrush(prefab.EditColor),
                Metadata = "",
                Base = prefab
            };
        }
    }
}