using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using EntityEditor.Annotations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class StringMap : IPropertyValue
    {
        public StringMap(JObject definition)
        {
            Values = new ObservableCollection<Row>();
            foreach (var kvp in definition)
            {
                Values.Add(new Row(kvp.Key, (string) kvp.Value));
            }
        }

        public ObservableCollection<Row> Values { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropMapStrStrTemplate"); }
        }

        public bool Locked { get; set; }

        public JToken Serialize()
        {
            var obj = new JObject();
            foreach (var row in Values)
            {
                obj[row.Key] = row.Value;
            }

            return obj;
        }

        public class Row : INotifyPropertyChanged, IEquatable<Row>
        {
            public Row()
                : this("", "")
            {
            }

            public Row(string key, string value)
            {
                _key = key;
                _value = value;
            }

            private string _key;
            private string _value;

            public string Key
            {
                get { return _key; }
                set
                {
                    if (value == _key) return;
                    _key = value;
                    OnPropertyChanged();
                }
            }

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

            public event PropertyChangedEventHandler PropertyChanged;

            [NotifyPropertyChangedInvocator]
            protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
            {
                var handler = PropertyChanged;
                if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
            }

            public bool Equals(Row other)
            {
                return Key == other.Key && Value == other.Value;
            }
        }

        public bool Equals(IPropertyValue other)
        {
            var m = other as StringMap;
            return 
                m != null && Values.Count == m.Values.Count && 
                Values.Zip(m.Values, (v1, v2) => v1.Equals(v2)).All(r => r);
        }
    }
}