using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class String : IPropertyValue
    {
        private const string DefaultTemplate = "PropStringTemplate";

        private static readonly Dictionary<string, string> Templates = new Dictionary<string, string>
        {
            {"texture", "PropStringTextureTemplate"}
        };

        private readonly string _semanticTemplate;

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

        public string Value { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource(_semanticTemplate); }
        }

        public JToken Serialize()
        {
            return Value;
        }
    }
}
