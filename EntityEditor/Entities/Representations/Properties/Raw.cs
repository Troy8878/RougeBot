using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Raw : IPropertyValue
    {
        public JToken JsonValue { get; set; }

        public string RawValue
        {
            get { return JsonValue.ToString(Formatting.Indented); }
            set { JsonValue = JToken.Parse(value); }
        }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropRawTemplate"); }
        }

        public bool Locked { get; set; }

        public JToken Serialize()
        {
            return JsonValue;
        }

        public bool Equals(IPropertyValue other)
        {
            var r = other as Raw;
            return r != null && JToken.DeepEquals(JsonValue, r.JsonValue);
        }
    }
}
