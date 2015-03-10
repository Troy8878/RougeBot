using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EntityEditor.Entities.Representations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Component : IEquatable<Component>
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

        public JObject Serialize()
        {
            var cdef = new JObject();
            foreach (var kvp in Properties)
            {
                cdef[kvp.Key] = kvp.Value.Serialize();
            }
            return cdef;
        }

        public bool Equals(Component other)
        {
            if (other == null)
                return false;

            if (Properties.Count != other.Properties.Count)
                return false;

            foreach (var p1 in Properties)
            {
                IPropertyValue p2;
                if (!other.Properties.TryGetValue(p1.Key, out p2))
                    return false;
                if (!p1.Value.Equals(p2))
                    return false;
            }

            return true;
        }
    }
}
