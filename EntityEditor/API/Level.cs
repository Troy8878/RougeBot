using EntityEditor.API.Variants;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EntityEditor.API
{
    public class Level : IVariant
    {
        public List<Component> RootComponents;
    }
}
