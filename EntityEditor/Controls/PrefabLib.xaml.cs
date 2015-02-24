using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using EntityEditor.Helpers;

namespace EntityEditor.Controls
{
    /// <summary>
    ///     Interaction logic for PrefabLib.xaml
    /// </summary>
    public partial class PrefabLib : UserControl
    {
        public PrefabLib()
        {
            InitializeComponent();
            Refresh(null, null);
        }

        public void Refresh(object sender, RoutedEventArgs e)
        {
            Tree.Items.Clear();

            var home = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            var libdir = Path.Combine(home, "Source/Repos/Game200-Project/Roguelike/Assets/Prefabs");
            if (!Directory.Exists(libdir))
                return;

            foreach (var dir in new DirectoryInfo(libdir).GetDirectories())
            {
                var node = new TreeViewItem {Header = dir.Name};
                foreach (var fileInfo in dir.GetFiles())
                {
                    var file = fileInfo;

                    var item = new TreeViewItem
                    {
                        Header = file.Name,
                        DataContext = file
                    };
                    item.Selected += delegate
                    {
                        var room = this.GetParent<PrefabRoom>();
                        room.SetFile(file.FullName);
                        using (var stream = File.OpenRead(file.FullName))
                        {
                            room.Load(stream);
                        }
                    };
                    node.Items.Add(item);
                }
                Tree.Items.Add(node);
            }
        }
    }
}