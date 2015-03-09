using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.Annotations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Enum : IPropertyValue, INotifyPropertyChanged
    {
        public Enum(string value, string[] options)
        {
            Options = options;
            Value = value;
        }

        private string _value;
        public string[] Options { get; private set; }

        public string Value
        {
            get { return _value; }
            set
            {
                if (value == _value) return;
                _value = value;
                OnPropertyChanged();
            }
        }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropEnumTemplate"); }
        }

        public JToken Serialize()
        {
            if (string.IsNullOrWhiteSpace(Value))
                return JValue.CreateNull();

            return Value;
        }

        public bool Locked { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public bool Equals(IPropertyValue other)
        {
            var e = other as Enum;
            return e != null && Value == e.Value;
        }
    }
}
