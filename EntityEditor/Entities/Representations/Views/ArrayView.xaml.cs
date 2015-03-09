using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace EntityEditor.Entities.Representations.Views
{
    /// <summary>
    /// Interaction logic for ArrayView.xaml
    /// </summary>
    public partial class ArrayView : UserControl
    {
        public ArrayView()
        {
            InitializeComponent();
        }

        private void AddClick(object sender, RoutedEventArgs e)
        {
            var ary = (Properties.Array) DataContext;
            ary.New();
        }

        private void RemoveClick(object sender, RoutedEventArgs e)
        {
            var ary = (Properties.Array) DataContext;
            var item = (IPropertyValue) ((FrameworkElement) sender).DataContext;
            ary.Remove(item);
        }

        private void UpClick(object sender, RoutedEventArgs e)
        {
            var ary = (Properties.Array) DataContext;
            var item = (IPropertyValue) ((FrameworkElement) sender).DataContext;
            ary.MoveBy(item, -1);
        }

        private void DownClick(object sender, RoutedEventArgs e)
        {
            var ary = (Properties.Array) DataContext;
            var item = (IPropertyValue) ((FrameworkElement) sender).DataContext;
            ary.MoveBy(item, 1);
        }
    }
}
