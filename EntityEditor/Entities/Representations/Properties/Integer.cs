using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Integer : IPropertyValue
    {
        public long? Value { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropIntegerTemplate"); }
        }

        public JToken Serialize()
        {
            return Value == null ? JValue.CreateNull() : new JValue(Value);
        }

        public bool Locked { get; set; }

        public bool Equals(IPropertyValue other)
        {
            var f = other as Integer;
            return f != null && Value == f.Value;
        }
    }
}