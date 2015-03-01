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
            
            CanEdit = true;
            Message = "";
        }

        public string RandomMessage
        {
            get { return MessageStrings[new Random().Next(0, MessageStrings.Length)]; }
        }

        private async void CommitClick(object sender, RoutedEventArgs e)
        {
            CanEdit = false;
            var message = Message;
            using (var repo = new Repository(MainWindow.Instance.RepoDir))
            {
                // Stage everything in the repo
                try
                {
                    await Task.Run(delegate
                    {
                        // This closure will never be disposed because we await the call
                        
                        // ReSharper disable AccessToDisposedClosure
                        StageDir(repo, MainWindow.Instance.RepoDir);
                        repo.Commit(message, new CommitOptions {PrettifyMessage = true});
                        // ReSharper restore AccessToDisposedClosure
                    });
                    Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            CanEdit = true;
        }

        private void StageDir(IRepository repo, string dir, string basepath = "")
        {
            var dirinfo = new DirectoryInfo(dir);
            if (dirinfo.Name.StartsWith(".") ||
                dirinfo.Name == "x64")
                return;

            foreach (var subdir in dirinfo.GetDirectories())
            {
                StageDir(repo, subdir.FullName, Path.Combine(basepath, subdir.Name));
            }
            
            SetStagingMessage(Path.Combine(basepath, dirinfo.Name));

            foreach (var file in dirinfo.GetFiles().Where(file => !file.Name.StartsWith(".")))
            {
                repo.Stage(file.FullName);
            }
        }

        private void SetStagingMessage(string file)
        {
            Dispatcher.Invoke(() => StagingFile = "Staging file: " + file);
        }

        public static readonly DependencyProperty StagingFileProperty = DependencyProperty.Register(
            "StagingFile", typeof (string), typeof (CommitMessage), new PropertyMetadata(default(string)));

        public string StagingFile
        {
            get { return (string) GetValue(StagingFileProperty); }
            set { SetValue(StagingFileProperty, value); }
        }

        public static readonly DependencyProperty MessageProperty = DependencyProperty.Register(
            "Message", typeof (string), typeof (CommitMessage), new PropertyMetadata(default(string)));

        public string Message
        {
            get { return (string) GetValue(MessageProperty); }
            set { SetValue(MessageProperty, value); }
        }

        public static readonly DependencyProperty CanEditProperty = DependencyProperty.Register(
            "CanEdit", typeof (bool), typeof (CommitMessage), new PropertyMetadata(default(bool)));

        public bool CanEdit
        {
            get { return (bool) GetValue(CanEditProperty); }
            set { SetValue(CanEditProperty, value); }
        }
    }
}