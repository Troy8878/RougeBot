using System.Collections.Generic;
using System.Windows;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations.Properties
{
    public class Vector : IPropertyValue
    {
        private const string DefaultTemplate = "PropVectorTemplate";

        private static readonly Dictionary<string, string> Templates = new Dictionary<string, string>
        {
            //{"range", "PropVectorRangeTemplate"},
            {"color", "PropVectorColorTemplate"}
        };

        private readonly string _semanticTemplate;

        public Vector(JArray field, int dimensions, string semanticTemplate)
        {
            if (semanticTemplate != null && Templates.ContainsKey(semanticTemplate))
            {
                _semanticTemplate = Templates[semanticTemplate];
            }
            else
            {
                _semanticTemplate = DefaultTemplate;
            }

            Dimensions = dimensions;

            if (Dimensions > 0 && field.Count > 0)
                X = (double) field[0];
            if (Dimensions > 1 && field.Count > 1)
                Y = (double) field[1];
            if (Dimensions > 2 && field.Count > 2)
                Z = (double) field[2];
            if (Dimensions > 3 && field.Count > 3)
                W = (double) field[3];
        }

        public double X { get; set; }
        public double Y { get; set; }
        public double Z { get; set; }
        public double W { get; set; }
        public int Dimensions { get; set; }

        public DataTemplate RenderTemplate
        {
            get { return (DataTemplate) Application.Current.FindResource(_semanticTemplate); }
        }

        public JToken Serialize()
        {
            var ary = new JArray();

            if (Dimensions > 0)
                ary.Add(X);
            if (Dimensions > 1)
                ary.Add(Y);
            if (Dimensions > 2)
                ary.Add(Z);
            if (Dimensions > 3)
                ary.Add(W);

            return ary;
        }

        public bool Locked { get; set; }
    }
}