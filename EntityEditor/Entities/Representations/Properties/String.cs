using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;
using EntityEditor.Annotations;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class String : IPropertyValue, INotifyPropertyChanged
    {
        private const string DefaultTemplate = "PropStringTemplate";

        private static readonly Dictionary<string, string> Templates = new Dictionary<string, string>
        {
            {"texture", "PropStringTextureTemplate"}
        };

        private readonly string _semanticTemplate;
        private string _value;

        public String(string semantics)
        {
            if (semantics != null && Templates.ContainsKey(semantics))
            {
                _semanticTemplate = Templates[semantics];
            }
            else
            {
                _semanticTemplate = DefaultTemplate;
            }
        }

        public string Value
        {
            get { return _value; }
            set
            {
                if (value == _value) return;
                _value = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource(_semanticTemplate); }
        }

        public JToken Serialize()
        {
            return Value;
        }

        public bool Locked { get; set; }

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}