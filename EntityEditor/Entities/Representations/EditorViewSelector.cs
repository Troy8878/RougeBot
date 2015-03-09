using System.Windows;
using System.Windows.Controls;
using EntityEditor.Entities.Project;

namespace EntityEditor.Entities.Representations
{
    public class EditorViewSelector : DataTemplateSelector
    {
        public DataTemplate Empty { get; set; }

        public DataTemplate Entity { get; set; }

        public DataTemplate Archetype { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            if (item is Entity)
            {
                return Entity;
            }
            if (item is Archetype)
            {
                return Archetype;
            }

            return Empty;
        }
    }
}