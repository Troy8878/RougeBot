using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.API;
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

        public async void Refresh()
        {
            try
            {
                if (MainWindow.Instance == null) return;

                var repo = new Repository(MainWindow.Instance.RepoDir);
                var data = new GitData
                {
                    Status = repo.RetrieveStatus(),
                    MasterBranch = repo.Branches[repo.Head.TrackedBranch.UpstreamBranchCanonicalName]
                };

                var details = data.MasterBranch.TrackingDetails;
                data.UpToDate = details.AheadBy == 0 && details.BehindBy == 0;
                data.Ahead = details.AheadBy;
                data.Behind = details.BehindBy;

                DataContext = data;

                await Task.Run(delegate
                {
                    var author = Author.Load();
                    repo.Fetch("origin", new FetchOptions
                    {
                        CredentialsProvider = (a, b, c) => author.Credentials
                    });

                    var ndetails = data.MasterBranch.TrackingDetails;
                    data.Ahead = ndetails.AheadBy;
                    data.Behind = ndetails.BehindBy;
                    Dispatcher.Invoke(() => DataContext = data);
                });
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.ToString());
            }
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
        public int? Ahead { get; set; }
        public int? Behind { get; set; }
        public bool UpToDate { get; set; }
    }
}