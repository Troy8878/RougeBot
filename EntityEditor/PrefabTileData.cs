using System;
using System.Windows.Media;

namespace EntityEditor
{
    public class PrefabTileData
    {
        public SolidColorBrush Color { get; set; }

        static readonly Random Lolrandom = new Random();
        public PrefabTileData()
        {
            Color = new SolidColorBrush(Lolrandom.NextDouble() > 0.5 ? Colors.Aqua : Colors.Blue);
        }
    }
}
