using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
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
using EntityEditor.Annotations;
using EntityEditor.Entities.Serialization;
using EntityEditor.Helpers;
using MahApps.Metro.Controls;

namespace EntityEditor.Entities.Representations.Views
{
    /// <summary>
    /// Interaction logic for ComponentSelector.xaml
    /// </summary>
    public partial class ComponentSelector : INotifyPropertyChanged
    {
        public ComponentSelector()
        {
            _filter = "";
            InitializeComponent();
        }

        public string Filter
        {
            get { return _filter; }
            set
            {
                if (value == _filter) return;
                _filter = value;
                OnPropertyChanged();
                OnPropertyChanged("FilteredComponents");
            }
        }

        public ComponentDefinition[] Components
        {
            get { return Editor.ComponentDefinitions; }
        }

        public IEnumerable<ComponentDefinition> FilteredComponents
        {
            get { return Components.Where(component => component.Name.ToLower().StartsWith(Filter.ToLower())); }
        }

        public ComponentDefinition SelectedComponent;
        private string _filter;

        public void Select()
        {
            Filter = "";
            SelectedComponent = null;

            var flyout = this.GetParent<Flyout>();
            flyout.IsOpen = true;
        }

        private void ChooseComponent(object sender, RoutedEventArgs e)
        {
            SelectedComponent = (ComponentDefinition) ((Button) sender).DataContext;
            var flyout = this.GetParent<Flyout>();
            flyout.IsOpen = false;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        private void TextBoxBase_OnTextChanged(object sender, TextChangedEventArgs e)
        {
            Filter = ((TextBox) sender).Text;
        }
    }
}
