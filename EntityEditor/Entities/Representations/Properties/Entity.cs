using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Entity : IPropertyValue
    {
        public Entity(JObject data)
        {
            Value = new Project.Entity(data);
        }

        public Project.Entity Value { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropEntityTemplate"); }
        }

        public JToken Serialize()
        {
            return Value.Serialize();
        }

        public bool Locked { get; set; }
        public bool Equals(IPropertyValue other)
        {
            var e = other as Entity;
            return e != null && Value.Equals(e.Value);
        }
    }
}
