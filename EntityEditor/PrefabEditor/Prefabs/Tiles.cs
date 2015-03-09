using System.Collections.Generic;
using System.Windows.Media;

namespace EntityEditor.PrefabEditor.Prefabs
{
    public class Tiles
    {
        public static readonly Dictionary<int, TilePrefab> Prefabs = new Dictionary<int, TilePrefab>();

        static Tiles()
        {
            AddBase(0, "Floor", Colors.SaddleBrown);
            AddBase(1, "Wall", Colors.DimGray);
            AddBase(2, "Enemy", Colors.DarkViolet);
            AddBase(3, "Bork Wall", Colors.SlateGray);
            AddBase(4, "Player Start", Colors.Firebrick);
            AddBase(5, "Item Spawn", Colors.DodgerBlue);
            AddBase(6, "Stairs", Colors.MediumPurple);
        }

        private static void AddBase(int id, string name, Color color)
        {
            AddBase(new TilePrefab {Id = id, Name = name, EditColor = color});
        }

        private static void AddBase(TilePrefab prefab)
        {
            Prefabs[prefab.Id] = prefab;
        }
    }
}