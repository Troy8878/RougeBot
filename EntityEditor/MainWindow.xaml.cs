using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.API;
using EntityEditor.Entities;
using EntityEditor.Properties;
using LibGit2Sharp;
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

        public static readonly DependencyProperty GitUnlockedProperty = DependencyProperty.Register(
            "GitUnlocked", typeof (bool), typeof (MainWindow), new PropertyMetadata(default(bool)));

        public string RepoDir = Settings.Default.RepoPath;

        public MainWindow()
        {
            Instance = this;

            GitUnlocked = true;

            if (!HasValidRepoDir())
                SelectCodeDir();
            UpdateRepoDirDisplay();

            InitializeComponent();
        }

        public bool GitUnlocked
        {
            get { return (bool) GetValue(GitUnlockedProperty); }
            set { SetValue(GitUnlockedProperty, value); }
        }

        private void ButtonPrefab(object sender, RoutedEventArgs e)
        {
            GitUnlocked = false;
            new PrefabEditor.PrefabEditor {Owner = this}.Show();
            GitUnlocked = true;
        }

        private void ButtonEntity(object sender, RoutedEventArgs e)
        {
            GitUnlocked = false;
            new Editor {Owner = this}.Show();
            GitUnlocked = true;
            GitView.Refresh();
        }

        private void UpdateRepoDirDisplay()
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                RepoDirDisplay.Text = RepoDir;
                GitView.Refresh();
            }));
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
                if (dlg.ShowDialog(this) != CommonFileDialogResult.Ok)
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

        private void CommitClick(object sender, RoutedEventArgs e)
        {
            CommitMessage.Reset();
            CommitFlyout.IsOpen = true;
        }

        private async void SyncClick(object sender, RoutedEventArgs e)
        {
            GitUnlocked = false;

            var author = Author.Load();
            if (!author.IsComplete)
            {
                new AuthorInput {Owner = this}.ShowDialog();
                author = Author.Load();
                if (!author.IsComplete)
                    return;
            }

            var status = new SyncProgress {Owner = this};
            status.Show();
            status.SetProgress(null);
            status.SetMessage("Preparing");

            try
            {
                await Task.Run(delegate
                {
                    using (var repo = new Repository(RepoDir))
                    {
                        if (repo.RetrieveStatus().Any(item => item.State != FileStatus.Ignored))
                        {
                            Dispatcher.Invoke(() => status.CloseProgress());
                            MessageBox.Show(
                                "You have uncommitted work. " +
                                "Please commit your work before syncing.");
                            return;
                        }

                        status.SetMessage("Pulling repository");

                        var result = repo.Network.Pull(author.GetSignature(), new PullOptions
                        {
                            FetchOptions = new FetchOptions
                            {
                                CredentialsProvider = (a, b, c) => author.Credentials,
                                OnTransferProgress = progress =>
                                {
                                    status.SetMessage(string.Format(
                                        "Fetching repository ({0}/{1} objects)",
                                        progress.ReceivedObjects,
                                        progress.TotalObjects));
                                    status.SetProgress(
                                        progress.ReceivedObjects/
                                        (double) progress.TotalObjects);
                                    return true;
                                }
                            },
                            MergeOptions = new MergeOptions
                            {
                                CommitOnSuccess = true,
                                OnCheckoutProgress = (path, curr, total) =>
                                {
                                    if (curr == total)
                                    {
                                        status.SetMessage("Pull complete");
                                        status.SetProgress(null);
                                        return;
                                    }

                                    status.SetMessage(string.Format(
                                        "Merging files ({0}/{1}) ({2})",
                                        curr, total, path));
                                    status.SetProgress(curr/(double) total);
                                }
                            }
                        });

                        status.SetMessage("Preparing to send");
                        status.SetProgress(null);

                        if (result.Status == MergeStatus.Conflicts)
                        {
                            Dispatcher.Invoke(() => status.CloseProgress());
                            MessageBox.Show(
                                "There were merge conflicts, please open the Visual Studio " +
                                "Team Explorer or GitExtensions to resolve them.");
                            return;
                        }

                        var options = new PushOptions
                        {
                            CredentialsProvider = delegate { return author.Credentials; },
                            OnPackBuilderProgress = (stage, curr, total) =>
                            {
                                if (total == 0)
                                {
                                    return true;
                                }

                                status.SetMessage(string.Format(
                                    "Packing objects ({0}/{1} objects)",
                                    curr, total));
                                status.SetProgress(curr/(double) total);

                                // Slowing things down for the user :)
                                if (total - curr < 4)
                                    Thread.Sleep(500);
                                if (total == curr)
                                    Thread.Sleep(500);

                                return true;
                            },
                            OnPushTransferProgress = (curr, total, bytes) =>
                            {
                                if (total == 0)
                                {
                                    return true;
                                }

                                status.SetMessage(string.Format(
                                    "Pushing to server ({0}/{1} objects)",
                                    curr, total));
                                status.SetProgress(curr/(double) total);

                                // Slowing things down for the user :)
                                if (total - curr < 4)
                                    Thread.Sleep(500);
                                if (total == curr)
                                    Thread.Sleep(500);

                                return true;
                            }
                        };

                        var branch = repo.Branches[repo.Head.TrackedBranch.UpstreamBranchCanonicalName];
                        repo.Network.Push(branch, options);
                    }
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("{0}: {1}", ex.GetType().Name, ex.Message));
            }

            GitUnlocked = true;
            status.CloseProgress();
            GitView.Refresh();
        }

        private void VisualStudio(object sender, RoutedEventArgs e)
        {
            Process.Start(Path.Combine(RepoDir, "Roguelike", "Roguelike.sln"));
        }

        private void RunGame(object sender, RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = Path.Combine(RepoDir, "Roguelike", "build.bat"),
                WorkingDirectory = Path.Combine(RepoDir, "Roguelike")
            });
        }

        private void CommitFlyoutOpenChanged(object sender, RoutedEventArgs e)
        {
            GitUnlocked = !CommitFlyout.IsOpen;

            if (GitUnlocked)
            {
                GitView.Refresh();
            }
        }

        private void CommitMessageOnClose()
        {
            CommitFlyout.IsOpen = false;
        }
    }
}