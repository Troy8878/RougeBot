using System.Collections.Generic;
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
        }

        public List<Entity> Children { get; set; }
        public string Name { get; set; }

        public string Type { get; set; }

        public IEnumerable<object> OwnedItems
        {
            get { return Children; }
        }
    }
}