using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using LibGit2Sharp;

namespace EntityEditor.Views
{
    /// <summary>
    ///     Interaction logic for GitHistory.xaml
    /// </summary>
    public partial class GitHistory
    {
        public static readonly DependencyProperty LoadingProperty = DependencyProperty.Register(
            "Loading", typeof (bool), typeof (GitHistory), new PropertyMetadata(default(bool)));

        public static readonly DependencyProperty ItemsProperty = DependencyProperty.Register(
            "Items", typeof (Commit[]), typeof (GitHistory),
            new PropertyMetadata(default(Commit[])));

        public GitHistory()
        {
            InitializeComponent();
        }

        public bool Loading
        {
            get { return (bool) GetValue(LoadingProperty); }
            set { SetValue(LoadingProperty, value); }
        }

        public Commit[] Items
        {
            get { return (Commit[]) GetValue(ItemsProperty); }
            set { SetValue(ItemsProperty, value); }
        }

        public async void Refresh()
        {
            if (Loading)
                return;

            Loading = true;
            await Task.Delay(500);
            Items = await Task.Run(delegate
            {
                if (MainWindow.Instance == null)
                    return null;

                var repo = new Repository(MainWindow.Instance.RepoDir);
                return repo.Commits.Take(200).ToArray();
            });
            Loading = false;
        }

        private void GitHistoryLoaded(object sender, RoutedEventArgs e)
        {
        }
    }
}