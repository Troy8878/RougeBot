using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Media;
using EntityEditor.Annotations;
using EntityEditor.PrefabEditor.Controls;
using Newtonsoft.Json.Linq;

namespace EntityEditor.PrefabEditor.Prefabs
{
    public class PrefabTileData : INotifyPropertyChanged, ICloneable
    {
        private SolidColorBrush _color;
        private string _metadata;
        private string _name;

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

        public object Clone()
        {
            var item = (PrefabTileData) Tiles.Prefabs[Base.Id];
            item.Metadata = Metadata;
            return item;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public void Reassign(PrefabTileData other)
        {
            Color = other.Color;
            Name = other.Name;
            Metadata = other.Metadata;
            Base = other.Base;
        }

        public JObject Serialize()
        {
            var obj = new JObject();
            obj["id"] = Base.Id;
            obj["meta"] = Metadata;
            return obj;
        }

        public void FromSerialized(JObject jtile)
        {
            Reassign((PrefabTileData) Tiles.Prefabs[(int) jtile["id"]]);
            Metadata = (string) jtile["meta"];
        }
    }

    public class PrefabChangeHistory
    {
        public PrefabRoom Room { get; set; }
        public PrefabTileData Data { get; set; }
        public PrefabTileData Prev { get; set; }
        public PrefabTileData Next { get; set; }

        public EditHistory.RedoAction Undo()
        {
            Data.Reassign(Prev);

            if (Room.HasFile())
                Room.Save();

            return Redo;
        }

        public EditHistory.UndoAction Redo()
        {
            Data.Reassign(Next);
            
            if (Room.HasFile())
                Room.Load();

            return Undo;
        }
    }
}