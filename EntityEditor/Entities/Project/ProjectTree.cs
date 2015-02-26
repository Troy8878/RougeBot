using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using EntityEditor.Annotations;

namespace EntityEditor.Entities.Project
{
    public class ProjectTree : INotifyPropertyChanged
    {
        private object _displayedItem;

        public ProjectTree()
        {
            Levels = new List<Level>();

            var leveldir = Path.Combine(
                MainWindow.Instance.RepoDir,
                "Roguelike\\Assets\\Levels");

            Archetypes = new ArchetypeContainer();

            foreach (var file in new DirectoryInfo(leveldir).GetFiles())
            {
                Levels.Add(new Level(file.Name, file.FullName));
            }
        }

        public List<Level> Levels { get; set; }
        public ArchetypeContainer Archetypes { get; set; }

        public object DisplayedItem
        {
            get { return _displayedItem; }
            set
            {
                if (Equals(value, _displayedItem)) return;
                _displayedItem = value;
                OnPropertyChanged();
            }
        }

        public IEnumerable<object> DisplayItems
        {
            get
            {
                var objects = new List<object> {Archetypes};
                objects.AddRange(Levels);
                return objects;
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