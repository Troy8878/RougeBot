using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using EntityEditor.Annotations;
using LibGit2Sharp;

namespace EntityEditor.Views
{
    public class HistoryItem : INotifyPropertyChanged
    {
        private bool _showList;

        public HistoryItem(Commit commit)
        {
            var repo = new Repository(MainWindow.Instance.RepoDir);

            Commit = commit;
            Changes = repo.Diff.Compare<TreeChanges>(commit.Parents.First().Tree, commit.Tree);
            ShowList = false;
        }

        public Commit Commit { get; set; }
        public TreeChanges Changes { get; set; }

        public bool ShowList
        {
            get { return _showList; }
            set
            {
                if (value.Equals(_showList)) return;
                _showList = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public static HistoryItem Make(Commit commit)
        {
            return new HistoryItem(commit);
        }

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}