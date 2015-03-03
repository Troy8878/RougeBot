using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Archetype : ITreeOwner, ISaveable
    {
        private string _file;

        public Archetype(FileSystemInfo file)
        {
            _file = file.FullName;
            Name = file.Name;
            Definition = JObject.Parse(File.ReadAllText(file.FullName));

            Components = new List<Component>();
            var jcomponents = Definition;
            if (jcomponents == null) return;

            foreach (var jcomp in jcomponents)
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

        public JObject Definition { get; set; }
        public List<Component> Components { get; set; }

        public void Save()
        {
            var data = new JObject();
            foreach (var comp in Components)
            {
                data[comp.Name] = comp.Serialize();
            }
            
            File.WriteAllText(_file, data.ToString(Formatting.Indented));
        }

        public string Name { get; set; }

        public string Type
        {
            get { return "Archetype"; }
        }

        public IEnumerable<object> OwnedItems
        {
            get { return new object[0]; }
        }

        public ITreeOwner Owner
        {
            get { return null; }
            set { }
        }
    }
}