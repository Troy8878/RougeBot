using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
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
            "Items", typeof (HistoryItem[]), typeof (GitHistory),
            new PropertyMetadata(default(HistoryItem[])));

        public GitHistory()
        {
            InitializeComponent();
        }

        public bool Loading
        {
            get { return (bool) GetValue(LoadingProperty); }
            set { SetValue(LoadingProperty, value); }
        }

        public HistoryItem[] Items
        {
            get { return (HistoryItem[]) GetValue(ItemsProperty); }
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
                return repo.Commits.Take(100).Select(HistoryItem.Make).ToArray();
            });
            Loading = false;
        }

        private void GitHistoryLoaded(object sender, RoutedEventArgs e)
        {
        }

        private void ShowChangesClicked(object sender, MouseButtonEventArgs e)
        {
            var item = (HistoryItem) ((FrameworkElement) sender).DataContext;
            item.ShowList = !item.ShowList;
        }
    }
}