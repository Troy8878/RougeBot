using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using EntityEditor.Annotations;

namespace EntityEditor.Entities.Representations
{
    public interface IPropertyValue : INotifyPropertyChanged
    {
        DataTemplate RenderTemplate { get; }
    }
}
