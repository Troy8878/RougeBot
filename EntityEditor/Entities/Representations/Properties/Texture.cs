using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using EntityEditor.Annotations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Texture : IPropertyValue, INotifyPropertyChanged
    {
        public Texture(JToken definition)
        {
            var obj = definition as JObject;
            if (definition.Type == JTokenType.String)
            {
                Value = new SingleTexture(new JObject {{"texture", (string) definition}});
            }
            else if (obj != null)
            {
                JToken zip;
                if (obj.TryGetValue("zipped", out zip))
                {
                    Value = new TextureZip(obj);
                }
                else
                {
                    Value = new SingleTexture(obj);
                }
            }
            else
            {
                Value = new SingleTexture(new JObject {{"texture", ""}});
            }
        }

        public IPropertyValue Value { get; private set; }
        public event PropertyChangedEventHandler PropertyChanged;
        public bool Locked { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return Value.RenderTemplate; }
        }

        public JToken Serialize()
        {
            return Value.Serialize();
        }

        public void ToZip()
        {
            var scalar = Value as SingleTexture;
            if (scalar == null) return;

            var texture = scalar.Value;
            Value = new TextureZip(new JObject {{"zipped", new JArray {texture}}});
        }

        public void ToScalar()
        {
            var zip = Value as TextureZip;
            if (zip == null) return;

            var texture = zip.Textures.First();
            Value = new SingleTexture(new JObject {{"texture", texture}});
        }

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public class SingleTexture : IPropertyValue, INotifyPropertyChanged
        {
            private string _value;

            public SingleTexture(JToken definition)
            {
                Value = (string) definition["texture"];
            }

            public string Value
            {
                get { return _value; }
                set
                {
                    if (value == _value) return;
                    _value = value;
                    OnPropertyChanged1();
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;

            public DataTemplate RenderTemplate
            {
                get { return (DataTemplate) Application.Current.FindResource("PropTextureSingleTemplate"); }
            }

            public JToken Serialize()
            {
                return Value;
            }

            public bool Locked { get; set; }

            [NotifyPropertyChangedInvocator]
            protected virtual void OnPropertyChanged1([CallerMemberName] string propertyName = null)
            {
                var handler = PropertyChanged;
                if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public class TextureZip : IPropertyValue, INotifyPropertyChanged
        {
            public TextureZip(JToken definition)
            {
                Textures = new ObservableCollection<string>();
            }

            public ObservableCollection<string> Textures { get; private set; }
            public event PropertyChangedEventHandler PropertyChanged;

            public DataTemplate RenderTemplate
            {
                get { return (DataTemplate) Application.Current.FindResource("PropTextureZipTemplate"); }
            }

            public JToken Serialize()
            {
                return new JObject
                {
                    {"zipped", new JArray(Textures)}
                };
            }

            public bool Locked { get; set; }

            [NotifyPropertyChangedInvocator]
            protected virtual void OnPropertyChanged1([CallerMemberName] string propertyName = null)
            {
                var handler = PropertyChanged;
                if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}