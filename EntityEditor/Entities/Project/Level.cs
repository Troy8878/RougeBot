using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Level : ITreeOwner
    {
        public Level(string name, string file)
        {
            Name = name;
            File = file;

            var leveldef = JObject.Parse(System.IO.File.ReadAllText(file));
            leveldef["children"] = leveldef["entities"];

            LevelRoot = new Entity(leveldef) {Name = "Root"};
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
    }
}