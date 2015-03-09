using System.Collections.Generic;
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
        public static Dictionary<string, ComponentDefinition> ComponentDefinitionsMap;

        public static Editor Instance;
        public static ProjectTree Project;

        static Editor()
        {
            ComponentDefinitions = JsonConvert.DeserializeObject<ComponentDefinition[]>
                (File.ReadAllText("Entities/serialize.json"));
            ComponentDefinitionsMap = new Dictionary<string, ComponentDefinition>();
            foreach (var definition in ComponentDefinitions)
            {
                ComponentDefinitionsMap[definition.Name] = definition;
            }
        }

        public Editor()
        {
            Instance = this;

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