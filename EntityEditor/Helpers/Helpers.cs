using System.Windows;
using System.Windows.Media;

namespace EntityEditor.Helpers
{
    public static class Helpers
    {
        public static T GetParent<T>(this DependencyObject depobj, string name = null) where T : FrameworkElement
        {
            var origFe = depobj as FrameworkElement;

            while (true)
            {
                if (depobj == null)
                    return GetFeParent<T>(origFe);

                var element = depobj as T;
                if (element != null && (name == null || element.Name == name))
                {
                    return element;
                }

                depobj = VisualTreeHelper.GetParent(depobj);
            }
        }

        private static T GetFeParent<T>(FrameworkElement fobj, string name = null) where T : FrameworkElement
        {
            while (true)
            {
                if (fobj == null)
                    return null;

                var element = fobj as T;
                if (element != null && (name == null || element.Name == name))
                {
                    return element;
                }

                fobj = fobj.Parent as FrameworkElement;
            }
        }
    }
}