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
using EntityEditor.Entities.Project;

namespace EntityEditor.Entities.Representations
{
    /// <summary>
    /// Interaction logic for ArchetypeView.xaml
    /// </summary>
    public partial class ArchetypeView : UserControl
    {
        public ArchetypeView()
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
    }
}
