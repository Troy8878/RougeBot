using System.Collections.Specialized;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using EntityEditor.Helpers;
using EntityEditor.Prefabs;
using EntityEditor.Properties;
using Microsoft.Win32;
using Newtonsoft.Json;
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

        private void AddImportExport()
        {
            var item = new MenuItem {Header = "_Export"};
            item.Click += delegate
            {
                var ary = new JArray();
                foreach (var libitem in Settings.Default.Library)
                {
                    ary.Add(libitem);
                }

                var dlg = new SaveFileDialog {Filter = "Prefab Library|*.prefablib.json"};
                if (dlg.ShowDialog() != true)
                    return;

                using (var file = dlg.OpenFile())
                using (var writer = new StreamWriter(file))
                {
                    writer.Write(ary.ToString());
                }
            };
            Items.Add(item);

            item = new MenuItem {Header = "_Import"};
            item.Click += delegate
            {
                var dlg = new OpenFileDialog {Filter = "Prefab Library|*.prefablib.json"};
                if (dlg.ShowDialog() != true)
                    return;

                var settings = Settings.Default;
                settings.Library = new StringCollection();

                using (var file = dlg.OpenFile())
                using (var reader = new StreamReader(file))
                {
                    var str = reader.ReadToEnd();
                    var data = JArray.Parse(str);
                    foreach (var libitem in data)
                    {
                        settings.Library.Add((string) libitem);
                    }
                }

                settings.Save();
                LoadLibrary();
            };
            Items.Add(item);
        }

        private void AddToLib(object sender, RoutedEventArgs routedEventArgs)
        {
            var room = this.GetParent<PrefabEditor>();
            var data = (PrefabTileData) room.Room.PaintTile.DataContext;
            var str = data.Serialize().ToString(Formatting.None);

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

            AddImportExport();
        }
    }
}