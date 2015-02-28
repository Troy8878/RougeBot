using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.Annotations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations
{
    public interface IPropertyValue
    {
        DataTemplate RenderTemplate { get; }
        JToken Serialize();

        bool Locked { get; set; }
    }
}
