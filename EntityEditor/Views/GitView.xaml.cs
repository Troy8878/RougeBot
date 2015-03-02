using System.Linq;
using System.Windows;
using LibGit2Sharp;

namespace EntityEditor.Views
{
    /// <summary>
    ///     Interaction logic for GitView.xaml
    /// </summary>
    public partial class GitView
    {
        public GitView()
        {
            Refresh();

            InitializeComponent();
        }

        public void Refresh()
        {
            if (MainWindow.Instance == null) return;

            var repo = new Repository(MainWindow.Instance.RepoDir);
            var data = new GitData
            {
                Status = repo.RetrieveStatus(),
                MasterBranch = repo.Branches["master"]
            };

            DataContext = data;
        }

        private void OnRefresh(object sender, RoutedEventArgs e)
        {
            Refresh();
        }
    }

    public class GitData
    {
        public RepositoryStatus Status { get; set; }
        public Branch MasterBranch { get; set; }
    }
}