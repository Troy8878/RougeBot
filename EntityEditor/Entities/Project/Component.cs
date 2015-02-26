using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EntityEditor.Entities.Representations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Component
    {
        public Component(string name, JObject definition)
        {
            Name = name;
            MissingData = false;
            Properties = new Dictionary<string, IPropertyValue>();

            if (!Editor.ComponentDefinitionsMap.ContainsKey(name))
            {
                MissingData = true;
                return;
            }

            Merge(definition, true);
        }

        public void Merge(JObject definition, bool first = false)
        {
            var cdef = Editor.ComponentDefinitionsMap[Name];
            foreach (var prop in cdef.Properties)
            {
                JToken value;
                if (definition.TryGetValue(prop.Property, out value))
                {
                    Properties[prop.Property] = PropertyFactory.Construct(Name, prop.Property, value);
                }
                else if (first)
                {
                    Properties[prop.Property] = PropertyFactory.Construct(Name, prop.Property, null);
                }
            }
        }

        public string Name { get; set; }

        public bool MissingData { get; set; }

        public Dictionary<String, IPropertyValue> Properties { get; set; } 
    }
}
