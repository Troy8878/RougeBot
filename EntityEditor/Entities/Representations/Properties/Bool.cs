using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Bool : IPropertyValue
    {
        public bool? Value { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropBoolTemplate"); }
        }

        public JToken Serialize()
        {
            return Value == null ? JValue.CreateNull() : new JValue(Value);
        }

        public bool Locked { get; set; }
    }
}