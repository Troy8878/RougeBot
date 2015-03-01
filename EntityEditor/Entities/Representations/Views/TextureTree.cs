using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace EntityEditor.Entities.Representations.Views
{
    public class TextureTree : TreeView
    {
        public static readonly DependencyProperty SelectedTextureProperty = DependencyProperty.Register(
            "SelectedTexture", typeof (String), typeof (TextureTree), new PropertyMetadata(default(String)));

        private bool _loaded;

        public TextureTree()
        {
            SelectedItemChanged += OnSelectedItemChanged;
        }

        public void Load()
        {
            if (_loaded)
                return;

            try
            {
                AddDir(
                    Path.Combine(MainWindow.Instance.RepoDir, "Roguelike", "Assets", "Textures"),
                    new string[] {}, this);
                _loaded = true;
            }
            catch (NullReferenceException)
            {
            }
        }

        public String SelectedTexture
        {
            get { return (String) GetValue(SelectedTextureProperty); }
            set { SetValue(SelectedTextureProperty, value); }
        }

        private void OnSelectedItemChanged(object sender,
            RoutedPropertyChangedEventArgs<object> e)
        {
            var node = (TreeViewItem) e.NewValue;
            var tex = node.DataContext as Texture;
            if (tex != null)
            {
                SelectedTexture = tex.Path;
            }
        }

        private static void AddDir(string path, string[] bpath, ItemsControl owner)
        {
            var dinfo = new DirectoryInfo(path);
            foreach (var dir in dinfo.GetDirectories())
            {
                var node = new TreeViewItem {Header = dir.Name};

                var npath = new[] {bpath, new[] {dir.Name}}.SelectMany(l => l).ToArray();
                AddDir(dir.FullName, npath, node);

                owner.Items.Add(node);
            }

            foreach (var tex in dinfo.GetFiles())
            {
                var node = new TreeViewItem {Header = tex.Name};
                var npath = new[] {bpath, new[] {tex.Name}}.SelectMany(l => l);
                node.DataContext = new Texture {Name = tex.Name, Path = string.Join("/", npath)};
                owner.Items.Add(node);
            }
        }

        public class Texture
        {
            public string Name;
            public string Path;
        }
    }
}