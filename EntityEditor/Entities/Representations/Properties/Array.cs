using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using EntityEditor.Entities.Serialization;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Array : IPropertyValue
    {
        private readonly ComponentDefinition.ComponentProperty _ptype;

        public Array(JArray def, ComponentDefinition.ComponentProperty prop)
        {
            Items = new ObservableCollection<IPropertyValue>();

            _ptype = new ComponentDefinition.ComponentProperty
            {
                Property = "",
                Usage = new ComponentDefinition.ComponentProperty.PropertyUsage
                {
                    Type = prop.Usage.ArrayContains
                }
            };

            foreach (var item in def)
            {
                Items.Add(PropertyFactory.ConstructType(_ptype, item));
            }
        }

        public ObservableCollection<IPropertyValue> Items { get; private set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource("PropArrayTemplate"); }
        }

        public JToken Serialize()
        {
            return new JArray(Items.Select(pv => pv.Serialize()));
        }

        public bool Locked { get; set; }
    }
}