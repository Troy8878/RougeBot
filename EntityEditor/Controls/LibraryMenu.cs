using System.Collections.Specialized;
using System.Windows;
using System.Windows.Controls;
using EntityEditor.Helpers;
using EntityEditor.Prefabs;
using EntityEditor.Properties;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Controls
{
    public class LibraryMenu : MenuItem
    {
        public LibraryMenu()
        {
            LoadLibrary();
        }

        private void AddAdder()
        {
            var item = new MenuItem {Header = "Add Paint"};
            item.Click += AddToLib;
            Items.Add(item);
        }

        private void AddToLib(object sender, RoutedEventArgs routedEventArgs)
        {
            var room = this.GetParent<PrefabEditor>();
            var data = (PrefabTileData) room.Room.PaintTile.DataContext;
            var str = data.Serialize().ToString();

            var settings = Settings.Default;
            if (settings.Library.Contains(str))
                return;

            settings.Library.Add(str);
            settings.Save();

            LoadLibrary();
        }

        private void LoadLibrary()
        {
            Items.Clear();

            AddAdder();

            var settings = Settings.Default;
            if (settings.Library == null)
            {
                settings.Library = new StringCollection();
                settings.Save();
            }

            foreach (var libitem in settings.Library)
            {
                var data = new PrefabTileData();
                data.FromSerialized(JObject.Parse(libitem));

                var item = new MenuItem
                {
                    Header = data.Name,
                    Background = data.Color
                };

                var use = new MenuItem {Header = "_Use"};
                use.Click += delegate
                {
                    var room = this.GetParent<PrefabEditor>();
                    var paintData = (PrefabTileData) room.Room.PaintTile.DataContext;

                    paintData.Reassign(data);
                };
                item.Items.Add(use);

                item.Items.Add(new MenuItem {Header = "_Meta: " + data.Metadata});

                var remove = new MenuItem {Header = "_Remove"};
                var str = libitem;
                remove.Click += delegate
                {
                    settings.Library.Remove(str);
                    settings.Save();
                    LoadLibrary();
                };
                item.Items.Add(remove);

                Items.Add(item);
            }
        }
    }
}