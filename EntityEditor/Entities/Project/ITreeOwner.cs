using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EntityEditor.Annotations;

namespace EntityEditor.Entities.Project
{
    public interface ITreeOwner
    {
        string Name { get; }
        string Type { get; }
        IEnumerable<object> OwnedItems { get; }
    }
}
