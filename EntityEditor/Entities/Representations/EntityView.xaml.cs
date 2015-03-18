using System;
using System.Windows;
using System.Windows.Controls;
using EntityEditor.Entities.Project;
using EntityEditor.Helpers;

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
            var editor = this.GetParent<Editor>();
            editor.ComponentSelector.Select();

            editor.ComponentFlyout.IsOpenChanged += ComponentFlyoutOnClose;
        }

        private void ComponentFlyoutOnClose(object sender, RoutedEventArgs routedEventArgs)
        {
            var editor = this.GetParent<Editor>();
            if (editor.ComponentFlyout.IsOpen)
                return;

            editor.ComponentFlyout.IsOpenChanged -= ComponentFlyoutOnClose;

            var component = editor.ComponentSelector.SelectedComponent;
            var entity = (Entity) DataContext;

            if (component == null)
                return;

            if (!entity.NewComponent(component.Name))
            {
                MessageBox.Show("Component already exists");
            }
        }

        private void RemoveComponent(object sender, RoutedEventArgs e)
        {
            ((Component) ((FrameworkElement) sender).DataContext).Remove();
        }
    }
}