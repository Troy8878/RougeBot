using System;
using System.ComponentModel;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Windows.Media;
using EntityEditor.Annotations;

namespace EntityEditor
{
    public class PrefabTileData : INotifyPropertyChanged
    {
        private static readonly Random Lolrandom = new Random();
        private SolidColorBrush _color;

        public PrefabTileData()
        {
            Randomize();
        }

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

        public event PropertyChangedEventHandler PropertyChanged;

        public void Randomize()
        {
            var colors = typeof (Colors).GetProperties();
            var index = Lolrandom.Next(colors.Length);

            Color = new SolidColorBrush((Color) colors[index].GetValue(null));
        }

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}