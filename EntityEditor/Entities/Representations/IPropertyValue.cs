using System;
using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations
{
    public interface IPropertyValue : IEquatable<IPropertyValue>
    {
        DataTemplate RenderTemplate { get; }
        bool Locked { get; set; }
        JToken Serialize();
    }
}