using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Archetype : ITreeOwner, IComponentOwner, ISaveable
    {
        private readonly string _file;

        public Archetype(FileSystemInfo file)
        {
            _file = file.FullName;
            Name = file.Name;
            Definition = JObject.Parse(File.ReadAllText(file.FullName));

            Components = new ObservableCollection<Component>();
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
                    var component = new Component(jcomp.Key, (JObject) jcomp.Value) {Owner = this};
                    Components.Add(component);
                }
            }
        }

        public JObject Definition { get; set; }
        public ObservableCollection<Component> Components { get; set; }

        public void Save()
        {
            var data = Serialize();
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

        public bool NewComponent(string name)
        {
            if (Components.Any(c => c.Name == name))
            {
                return false;
            }

            var component = new Component(name, new JObject()) {Owner = this};
            if (Components.Any(c => c.Name == "ChildHierarchy"))
            {
                var index = Components.Count - 1;
                Components.Insert(index, component);
            }
            else
            {
                Components.Add(component);
            }

            return true;
        }

        public void RemoveComponent(string name)
        {
            var component = Components.FirstOrDefault(c => c.Name == name);
            Components.Remove(component);
        }

        public JObject Serialize()
        {
            var data = new JObject();
            foreach (var comp in Components)
            {
                data[comp.Name] = comp.Serialize();
            }
            return data;
        }
    }
}