using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Level : ITreeOwner, ISaveable
    {
        public Level(string name, string file)
        {
            Name = name;
            File = file;

            var leveldef = JObject.Parse(System.IO.File.ReadAllText(file));
            leveldef["children"] = leveldef["entities"];

            LevelRoot = new Entity(leveldef) {Name = Name, Owner = this};
        }
        
        public string Name { get; set; }

        public string Type
        {
            get { return "Level"; }
        }

        public string File { get; set; }
        public Entity LevelRoot { get; set; }

        public IEnumerable<object> OwnedItems
        {
            get { return new[] {LevelRoot}; }
        }

        public ITreeOwner Owner
        {
            get { return null; }
            set { }
        }

        public void Save()
        {
            var data = new JObject();
            data["name"] = Name;
            data["components"] = LevelRoot.SerializeComponents();
            data["entities"] = LevelRoot.SerializeChildren();

            System.IO.File.WriteAllText(File, data.ToString(Formatting.Indented));
        }
    }
}