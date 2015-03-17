using System;
using System.Windows;
using System.Windows.Controls;
using EntityEditor.Entities.Project;

namespace EntityEditor.Entities.Representations
{
    /// <summary>
    ///     Interaction logic for EntityView.xaml
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

        private void RemoveComponent(object sender, RoutedEventArgs e)
        {
            ((Component) ((FrameworkElement) sender).DataContext).Remove();
        }
    }
}