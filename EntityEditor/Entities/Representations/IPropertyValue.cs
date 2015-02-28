using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations
{
    public interface IPropertyValue
    {
        DataTemplate RenderTemplate { get; }
        bool Locked { get; set; }
        JToken Serialize();
    }
}