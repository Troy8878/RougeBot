using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace EntityEditor.Helpers
{
    public class NIntPluralConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object word, CultureInfo culture)
        {
            var i = ((int?) value) ?? 0;

            return string.Format(i == 1 ? "{0} {1}" : "{0} {1}s", i, word);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
