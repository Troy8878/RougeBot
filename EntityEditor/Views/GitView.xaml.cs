using System.Linq;
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
            DataContext = new Repository(MainWindow.Instance.RepoDir).RetrieveStatus();
            InitializeComponent();
        }

        public void Refresh()
        {
            DataContext = new Repository(MainWindow.Instance.RepoDir).RetrieveStatus();
        }
    }
}