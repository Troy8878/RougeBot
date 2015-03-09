using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using EntityEditor.Helpers;
using EntityEditor.Properties;

namespace EntityEditor.PrefabEditor.Controls
{
    /// <summary>
    ///     Interaction logic for PrefabLib.xaml
    /// </summary>
    public partial class PrefabLib
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
            var repoPath = MainWindow.Instance.RepoDir;
            const string repoDir = "Roguelike/Assets/Prefabs";
            var libdir = Path.Combine(home, repoPath, repoDir);
            if (!Directory.Exists(libdir))
            {
                return;
            }

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
                        using (var stream = room.OpenFileRead(file.FullName))
                        {
                            room.Load(stream);
                        }
                    };
                    node.Items.Add(item);
                    node.ExpandSubtree();
                }
                Tree.Items.Add(node);
            }
        }
    }
}