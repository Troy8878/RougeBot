using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Controls;
using System.Windows.Media;
using EntityEditor.Prefabs;

namespace EntityEditor.Controls
{
    /// <summary>
    ///     Follow steps 1a or 1b and then 2 to use this custom control in a XAML file.
    ///     Step 1a) Using this custom control in a XAML file that exists in the current project.
    ///     Add this XmlNamespace attribute to the root element of the markup file where it is
    ///     to be used:
    ///     xmlns:MyNamespace="clr-namespace:EntityEditor.Controls"
    ///     Step 1b) Using this custom control in a XAML file that exists in a different project.
    ///     Add this XmlNamespace attribute to the root element of the markup file where it is
    ///     to be used:
    ///     xmlns:MyNamespace="clr-namespace:EntityEditor.Controls;assembly=EntityEditor.Controls"
    ///     You will also need to add a project reference from the project where the XAML file lives
    ///     to this project and Rebuild to avoid compilation errors:
    ///     Right click on the target project in the Solution Explorer and
    ///     "Add Reference"->"Projects"->[Browse to and select this project]
    ///     Step 2)
    ///     Go ahead and use your control in the XAML file.
    ///     <MyNamespace:TileContext />
    /// </summary>
    public sealed class TileContext : ContextMenu
    {
        public bool SkipEmpty { get; set; }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);
            
            AddMeta();

            IEnumerable<KeyValuePair<int, TilePrefab>> prefabs = Tiles.Prefabs;
            if (SkipEmpty)
                prefabs = prefabs.Skip(1);
            foreach (var kvp in prefabs)
            {
                AddTile(kvp.Value);
            }
        }

        private void AddMeta()
        {
            var item = new MenuItem()
            {
                Header = "Set Metadata"
            };
            item.Click += delegate
            {
                var prefabTile = PlacementTarget as PrefabTile;
                if (prefabTile == null)
                    return;
                var tileData = prefabTile.DataContext as PrefabTileData;
                if (tileData == null)
                    return;

                tileData.Metadata = SetMetadata.GetString(tileData.Metadata);
            };
            AddChild(item);
        }

        private void AddTile(TilePrefab prefab)
        {
            var item = new MenuItem
            {
                Header = prefab.Name,
                Background = new SolidColorBrush(prefab.EditColor)
            };
            item.Click += delegate
            {
                var prefabTile = PlacementTarget as PrefabTile;
                if (prefabTile == null)
                    return;

                ((PrefabTileData)prefabTile.DataContext).Reassign((PrefabTileData) prefab);
            };
            AddChild(item);
        }
    }
}