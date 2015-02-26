using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EntityEditor.Entities.Project
{
    public class Archetype : ITreeOwner
    {
        public Archetype(FileInfo file)
        {
            Name = file.Name;
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
    }
}
