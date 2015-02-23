using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Media;
using EntityEditor.Annotations;

namespace EntityEditor.Prefabs
{
    public class PrefabTileData : INotifyPropertyChanged, ICloneable
    {
        private static readonly Random Lolrandom = new Random();
        private SolidColorBrush _color;
        private string _name;
        private string _metadata;

        public SolidColorBrush Color
        {
            get { return _color; }
            set
            {
                if (Equals(value, _color)) return;
                _color = value;
                OnPropertyChanged();
            }
        }

        public string Name
        {
            get { return _name; }
            set
            {
                if (value == _name) return;
                _name = value;
                OnPropertyChanged();
            }
        }

        public string Metadata
        {
            get { return _metadata; }
            set
            {
                if (value == _metadata) return;
                _metadata = value;
                OnPropertyChanged();
            }
        }

        public TilePrefab Base { get; set; }
        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public object Clone()
        {
            var item = (PrefabTileData) Tiles.Prefabs[Base.Id];
            item.Metadata = Metadata;
            return item;
        }
    }
}