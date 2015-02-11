using System.ComponentModel;
using System.Runtime.CompilerServices;
using EntityEditor.Annotations;

namespace EntityEditor
{
    public enum PrefabTileId
    {
        Empty,
        Wall,
        EnemySpawn,
        ChestSpawn
    };

    public class PrefabTileType : INotifyPropertyChanged
    {
        private PrefabTileId _id;
        private int _extra;

        public PrefabTileType(PrefabTileId id, int extra)
        {
            Id = id;
            Extra = extra;
        }

        public PrefabTileId Id
        {
            get { return _id; }
            set
            {
                if (value == _id) return;
                _id = value;
                OnPropertyChanged();
            }
        }

        public int Extra
        {
            get { return _extra; }
            set
            {
                if (value == _extra) return;
                _extra = value;
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