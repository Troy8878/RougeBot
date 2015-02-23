using System.Windows;
using System.Windows.Media;

namespace EntityEditor.Helpers
{
    public static class Helpers
    {
        public static T GetParent<T>(this DependencyObject depobj, string name = null) where T : FrameworkElement
        {
            while (true)
            {
                if (depobj == null)
                    return null;

                var element = depobj as T;
                if (element != null && (name == null || element.Name == name))
                {
                    return element;
                }

                depobj = VisualTreeHelper.GetParent(depobj);
            }
        }
    }
}