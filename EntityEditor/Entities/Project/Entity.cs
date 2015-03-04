using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Input;
using EntityEditor.Entities.Representations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Entity : ITreeOwner, IEquatable<Entity>, IEquatable<Archetype>, IEquatable<List<Component>>
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
                foreach (var jcomp in (JObject) jcomponents)
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

        public List<Entity> Children { get; set; }
        public List<Component> Components { get; set; }

        public string Name { get; set; }
        public string Type { get; set; }

        public IEnumerable<object> OwnedItems
        {
            get { return Children; }
        }

        public ITreeOwner Owner { get; set; }

        public JToken Serialize()
        {
            var data = new JObject();
            if (Name != "<UNNAMED>")
                data["name"] = Name;

            data["components"] = SerializeComponents();
            data["children"] = SerializeChildren();
            data["archetype"] = Type;
            return data;
        }

        public JToken SerializeChildren()
        {
            var children = new JArray();
            foreach (var child in Children)
            {
                children.Add(child.Serialize());
            }
            return children;
        }

        public JToken SerializeComponents()
        {
            var data = new JObject();
            var archetype = Editor.Project.Archetypes.Find(Type);

            foreach (var component in Components)
            {
                var archcomp = archetype.Components.FirstOrDefault(c => c.Name == component.Name);
                var cdata = new JObject();

                foreach (var prop in component.Properties)
                {
                    if (archcomp != null)
                    {
                        var archprop = archcomp.Properties
                            .Select(a => new KeyValuePair<string, IPropertyValue>?(a))
                            .FirstOrDefault(kvp => kvp != null && kvp.Value.Key == prop.Key);
                        if (archprop != null)
                        {
                            var ap = archprop.Value;
                            if (ap.Value.Equals(prop.Value))
                                continue;
                        }
                    }

                    cdata[prop.Key] = prop.Value.Serialize();
                }

                data[component.Name] = cdata;
            }

            return data;
        }

        public bool Equals(Entity other)
        {
            return
                other != null &&
                Name == other.Name &&
                Equals(other.Components) &&
                Children.Zip(other.Children, (c1, c2) => c1.Equals(c2)).All(r => r);
        }

        public bool Equals(Archetype other)
        {
            return other != null && Equals(other.Components);
        }

        public bool Equals(List<Component> other)
        {
            return (from c1 in Components
                    let c2 = other.FirstOrDefault(c => c.Name == c1.Name)
                    select c1.Equals(c2)).All(r => r);
        }
    }
}