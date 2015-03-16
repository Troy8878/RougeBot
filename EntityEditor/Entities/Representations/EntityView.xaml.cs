using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using EntityEditor.Entities.Project;
using EntityEditor.Helpers;

namespace EntityEditor.Entities.Representations
{
    /// <summary>
    /// Interaction logic for EntityView.xaml
    /// </summary>
    public partial class EntityView
    {
        public EntityView()
        {
            InitializeComponent();
        }

        private void Save(object sender, RoutedEventArgs e)
        {
            var tree = (ITreeOwner) DataContext;
            while (tree.Owner != null)
            {
                tree = tree.Owner;
            }

            var saveable = tree as ISaveable;
            if (saveable != null)
            {
                saveable.Save();
            }
            else
            {
                var uie = ((UIElement) sender);
                ((Panel) LogicalTreeHelper.GetParent(uie)).Children.Remove(uie);
            }
        }

        private void AddComponentClick(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
    }
}
