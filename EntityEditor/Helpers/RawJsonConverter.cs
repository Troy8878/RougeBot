using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Helpers
{
    [ValueConversion(typeof(JToken), typeof(String))]
    public class RawJsonConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((JToken) value).ToString(Formatting.Indented);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                return JToken.Parse((string) value);
            }
            catch (Exception ex)
            {
                throw new FormatException(ex.Message);
            }
        }
    }
}
