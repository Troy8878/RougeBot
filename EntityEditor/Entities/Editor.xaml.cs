using System.IO;
using System.Windows;
using EntityEditor.Entities.Project;
using EntityEditor.Entities.Serialization;
using Newtonsoft.Json;

namespace EntityEditor.Entities
{
    /// <summary>
    ///     Interaction logic for Editor.xaml
    /// </summary>
    public partial class Editor
    {
        public static ComponentDefinition[] ComponentDefinitions;

        public ProjectTree Project;

        static Editor()
        {
            ComponentDefinitions = JsonConvert.DeserializeObject<ComponentDefinition[]>
                (File.ReadAllText("Entities/serialize.json"));
        }

        public Editor()
        {
            InitializeComponent();
            DataContext = Project = new ProjectTree();
        }

        private void OnSelected(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            var value = e.NewValue;
            if (value is Entity || value is Archetype)
                Project.DisplayedItem = value;

        }
    }
}