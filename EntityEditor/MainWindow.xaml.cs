using System.IO;
using System.Net.Configuration;
using System.Windows;
using EntityEditor.Entities;
using EntityEditor.Properties;
using Microsoft.WindowsAPICodePack.Dialogs;
using MessageBox = System.Windows.Forms.MessageBox;

namespace EntityEditor
{
    /// <summary>
    ///     Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public static MainWindow Instance;
        public string RepoDir = Settings.Default.RepoPath;

        public MainWindow()
        {
            Instance = this;

            InitializeComponent();
            
            if (!HasValidRepoDir())
                SelectCodeDir();
            UpdateRepoDirDisplay();
        }

        private void ButtonPrefab(object sender, RoutedEventArgs e)
        {
            new PrefabEditor.PrefabEditor().Show();
        }

        private void ButtonEntity(object sender, RoutedEventArgs e)
        {
            new Editor().Show();
        }

        private void UpdateRepoDirDisplay()
        {
            RepoDirDisplay.Text = RepoDir;
        }

        private static bool HasValidRepoDir(string dir = null)
        {
            if (dir == null)
            {
                dir = Settings.Default.RepoPath;
            }

            return
                Directory.Exists(Path.Combine(dir, ".git")) &&
                Directory.Exists(Path.Combine(dir, "Roguelike\\Assets"));
        }

        private void SelectCodeDir(bool init = true)
        {
            while (true)
            {
                var dlg = new CommonOpenFileDialog
                {
                    IsFolderPicker = true,
                    Title = "Select Roguebot git repo folder"
                };
                if (dlg.ShowDialog() != CommonFileDialogResult.Ok)
                {
                    if (init)
                        Loaded += (o, e) => Close();
                    return;
                }
                var dir = dlg.FileName;

                if (!HasValidRepoDir(dir))
                {
                    MessageBox.Show("That is not the game repo!");
                    continue;
                }

                RepoDir = dir;
                var settings = Settings.Default;
                settings.RepoPath = RepoDir;
                settings.Save();
                break;
            }
        }

        private void ChangeDirClick(object sender, RoutedEventArgs e)
        {
            SelectCodeDir();
            UpdateRepoDirDisplay();
        }
    }
}