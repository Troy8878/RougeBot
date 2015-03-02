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
            if (MainWindow.Instance != null)
                DataContext = new Repository(MainWindow.Instance.RepoDir).RetrieveStatus();

            InitializeComponent();
        }

        public void Refresh()
        {
            if (MainWindow.Instance != null)
                DataContext = new Repository(MainWindow.Instance.RepoDir).RetrieveStatus();
        }

        private void OnRefresh(object sender, RoutedEventArgs e)
        {
            Refresh();
        }
    }
}