using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Float : IPropertyValue
    {
        public float? Value { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropFloatTemplate"); }
        }

        public JToken Serialize()
        {
            return Value == null ? JValue.CreateNull() : new JValue(Value);
        }

        public bool Locked { get; set; }
        public bool Equals(IPropertyValue other)
        {
            var f = other as Float;
            return f != null && Value == f.Value;
        }
    }
}
