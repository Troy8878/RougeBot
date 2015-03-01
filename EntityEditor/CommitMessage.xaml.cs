using System;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Policy;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.Annotations;
using LibGit2Sharp;

namespace EntityEditor
{
    /// <summary>
    ///     Interaction logic for CommitMessage.xaml
    /// </summary>
    public partial class CommitMessage
    {
        private static readonly string[] MessageStrings =
        {
            "What I learned in boating school is...",
            "Today I learned...",
            "Dear Princess Celestia...",
            "What really happened is...",
            "I told grandpa git that...",
            "These random messages are weird, but today...",
            "Connor was bored, so he gave me a random prompt and I said...",
            "I went on an adventure and...",
            "You have 45 minutes for this section of the SAT. You may begin...",
        };

        public CommitMessage()
        {
            InitializeComponent();
        }

        public string RandomMessage
        {
            get { return MessageStrings[new Random().Next(0, MessageStrings.Length)]; }
        }

        private async void CommitClick(object sender, RoutedEventArgs e)
        {
            var commitMessage = Message.Text;
            var repo = new Repository(MainWindow.Instance.RepoDir);

            // Stage everything in the repo
            try
            {
                await Task.Run(delegate
                {
                    StageDir(repo, MainWindow.Instance.RepoDir);
                    repo.Commit(commitMessage, new CommitOptions {PrettifyMessage = true});
                });
                Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void StageDir(IRepository repo, string dir, string basepath = "")
        {
            var dirinfo = new DirectoryInfo(dir);
            if (dirinfo.Name.StartsWith("."))
                return;

            foreach (var subdir in dirinfo.GetDirectories())
            {
                StageDir(repo, subdir.FullName, Path.Combine(basepath, subdir.Name));
            }

            foreach (var file in dirinfo.GetFiles().Where(file => !file.Name.StartsWith(".")))
            {
                SetStagingMessage(Path.Combine(basepath, file.Name));
                repo.Stage(file.FullName);
            }
        }

        private void SetStagingMessage(string file)
        {
            Dispatcher.Invoke(() => StagingFile = file);
        }

        public static readonly DependencyProperty StagingFileProperty = DependencyProperty.Register(
            "StagingFile", typeof (string), typeof (CommitMessage), new PropertyMetadata(default(string)));

        public string StagingFile
        {
            get { return (string) GetValue(StagingFileProperty); }
            set { SetValue(StagingFileProperty, value); }
        }
    }
}