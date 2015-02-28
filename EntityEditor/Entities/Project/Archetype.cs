using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Archetype : ITreeOwner
    {
        public Archetype(FileInfo file)
        {
            Name = file.Name;
            Definition = JObject.Parse(File.ReadAllText(file.FullName));

            Components = new List<Component>();
            var jcomponents = Definition;
            if (jcomponents != null)
            {
                foreach (var jcomp in (JObject)jcomponents)
                {
                    var existing = Components.FirstOrDefault(c => c.Name == jcomp.Key);
                    if (existing != null)
                    {
                        existing.Merge((JObject) jcomp.Value);
                    }
                    else
                    {
                        var component = new Component(jcomp.Key, (JObject) jcomp.Value);
                        Components.Add(component);
                    }
                }
            }
        }

        public JObject Definition { get; set; }
        public List<Component> Components { get; set; }
        public string Name { get; set; }

        public string Type
        {
            get { return "Archetype"; }
        }

        public IEnumerable<object> OwnedItems
        {
            get { return new object[0]; }
        }
    }
}