using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Entity : ITreeOwner
    {
        public Entity(JObject definition)
        {
            var jname = definition["name"];
            if (jname != null)
                Name = (string) jname;
            else
                Name = "<UNNAMED>";

            var jarch = definition["archetype"];
            if (jarch != null)
                Type = (string) jarch;
            else
                Type = "NoArchetype";

            Children = new List<Entity>();

            var jchildren = definition["children"];
            if (jchildren != null)
            {
                foreach (var child in jchildren)
                {
                    Children.Add(new Entity((JObject) child));
                }
            }

            Components = new List<Component>();

            var arch = new Archetype(new FileInfo(Path.Combine(
                MainWindow.Instance.RepoDir, "Roguelike", "Assets", "Entities", Type + ".entitydef")));

            foreach (var jcomp in arch.Definition)
            {
                var component = new Component(jcomp.Key, (JObject) jcomp.Value);
                Components.Add(component);
            }

            var jcomponents = definition["components"];
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

        public string Name { get; set; }

        public string Type { get; set; }

        public List<Entity> Children { get; set; }

        public List<Component> Components { get; set; } 

        public IEnumerable<object> OwnedItems
        {
            get { return Children; }
        }

        public JToken Serialize()
        {
            return JValue.CreateNull();
        }
    }
}