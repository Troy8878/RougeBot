using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class StringMap : IPropertyValue
    {
        private StringMap(JObject definition)
        {
            Values = new ObservableCollection<KeyValuePair<string, string>>();
            foreach (var kvp in definition)
            {
                Values.Add(new KeyValuePair<string, string>(kvp.Key, (string) kvp.Value));
            }
        }

        public ObservableCollection<KeyValuePair<string, string>> Values { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropMapStrStrTemplate"); }
        }

        public bool Locked { get; set; }

        public JToken Serialize()
        {
            return new JObject(Values);
        }
    }
}