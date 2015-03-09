using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace EntityEditor.Helpers
{
    public class VisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var visible = (bool) value;
            if (parameter as string == "Negate")
                visible = !visible;
            return visible ? Visibility.Visible : Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var visible = ((Visibility) value) == Visibility.Visible;
            if (parameter as string == "Negate")
                visible = !visible;
            return visible;
        }
    }
}
