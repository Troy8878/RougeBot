using System;
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
                if (item.Type == JTokenType.Comment)
                    continue;
                Items.Add(PropertyFactory.ConstructType(_ptype, item));
            }
        }

        public void New()
        {
            Items.Add(PropertyFactory.ConstructType(_ptype, null));
        }

        public void Add(IPropertyValue value)
        {
            Items.Add(value);
        }

        public void Move(IPropertyValue value, int index)
        {
            Items.Remove(value);
            if (index < 0 || index > Items.Count)
                throw new Exception("Tried to move value into invalid index");

            Items.Insert(index, value);
        }

        public void MoveBy(IPropertyValue value, int offset)
        {
            var index = Items.IndexOf(value) + offset;
            if (index < 0)
                index = 0;
            else if (index >= Items.Count)
                index = Items.Count - 1;

            Move(value, index);
        }

        public void Remove(IPropertyValue value)
        {
            Items.Remove(value);
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
        public bool Equals(IPropertyValue other)
        {
            var ary = other as Array;
            return ary != null && Items.Zip(ary.Items, (v1, v2) => v1.Equals(v2)).All(r => r);
        }
    }
}