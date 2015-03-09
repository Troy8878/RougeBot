using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.Annotations;
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
            var data = new GitData();

            var oldData = DataContext as GitData;
            if (oldData != null)
            {
                data = oldData;
            }

            data.Syncing = true;
            DataContext = data;

            try
            {
                if (MainWindow.Instance == null) return;
                
                await Task.Run(delegate
                {
                    var repo = new Repository(MainWindow.Instance.RepoDir);

                    var status = repo.RetrieveStatus();
                    var master = repo.Branches[repo.Head.TrackedBranch.UpstreamBranchCanonicalName];
                    var details = master.TrackingDetails;
                    var uptodate = details.AheadBy == 0 && details.BehindBy == 0;
                    var ahead = details.AheadBy;
                    var behind = details.BehindBy;
                    
                    Dispatcher.Invoke(delegate
                    {
                        data.Status = status;
                        data.MasterBranch = master;
                        data.UpToDate = uptodate;
                        data.Ahead = ahead;
                        data.Behind = behind;
                    });

                    var author = Author.Load();
                    repo.Fetch("origin", new FetchOptions
                    {
                        CredentialsProvider = (a, b, c) => author.Credentials
                    });

                    var ndetails = data.MasterBranch.TrackingDetails;
                    Dispatcher.Invoke(delegate
                    {
                        data.Ahead = ndetails.AheadBy;
                        data.Behind = ndetails.BehindBy;
                    });
                });
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.ToString());
            }
            finally
            {
                data.Syncing = false;
            }
        }

        private void OnRefresh(object sender, RoutedEventArgs e)
        {
            Refresh();
        }
    }

    public class GitData : INotifyPropertyChanged
    {
        private int? _ahead;
        private int? _behind;
        private Branch _masterBranch;
        private RepositoryStatus _status;
        private bool _syncing;
        private bool _upToDate;

        public RepositoryStatus Status
        {
            get { return _status; }
            set
            {
                if (Equals(value, _status)) return;
                _status = value;
                OnPropertyChanged();
            }
        }

        public Branch MasterBranch
        {
            get { return _masterBranch; }
            set
            {
                if (Equals(value, _masterBranch)) return;
                _masterBranch = value;
                OnPropertyChanged();
            }
        }

        public int? Ahead
        {
            get { return _ahead; }
            set
            {
                if (value == _ahead) return;
                _ahead = value;
                OnPropertyChanged();
            }
        }

        public int? Behind
        {
            get { return _behind; }
            set
            {
                if (value == _behind) return;
                _behind = value;
                OnPropertyChanged();
            }
        }

        public bool UpToDate
        {
            get { return _upToDate; }
            set
            {
                if (value.Equals(_upToDate)) return;
                _upToDate = value;
                OnPropertyChanged();
            }
        }

        public bool Syncing
        {
            get { return _syncing; }
            set
            {
                if (value.Equals(_syncing)) return;
                _syncing = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}