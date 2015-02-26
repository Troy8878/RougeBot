using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Project
{
    public class Archetype : ITreeOwner
    {
        public Archetype(FileInfo file)
        {
            Name = file.Name;
            Definition = JObject.Parse(File.ReadAllText(file.FullName));
        }

        public string Name { get; set; }

        public JObject Definition { get; set; }

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
