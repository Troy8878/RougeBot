using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EntityEditor.Entities.Project
{
    public interface IComponentOwner
    {
        bool NewComponent(string name);
        void RemoveComponent(string name);
    }
}
