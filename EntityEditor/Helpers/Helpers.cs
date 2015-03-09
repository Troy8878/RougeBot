using System;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Media;
using Newtonsoft.Json.Linq;

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

        public static T Apply<T>(this T obj, Action<T> action)
        {
            action(obj);
            return obj;
        }

        public static JArray HexToColor(this string str)
        {
            var ary = new JArray();
            while (str.Length >= 2)
            {
                var value = int.Parse(str.Substring(0, 2), NumberStyles.HexNumber);
                ary.Add(value/255.0f);

                str = str.Substring(2);
            }
            return ary;
        }

        public static Color NamedColor(this string str)
        {
            var value =
                (from color in typeof (Colors).GetFields(BindingFlags.Static)
                    where color.Name.Equals(str, StringComparison.InvariantCultureIgnoreCase)
                    select (Color) color.GetValue(null)).FirstOrDefault();

            value.ScA = 1.0f;
            return value;
        }
    }
}