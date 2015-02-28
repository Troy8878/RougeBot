using System.Collections.Generic;
using System.Linq;
using EntityEditor.Entities.Representations.Properties;
using EntityEditor.Entities.Serialization;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Representations
{
    public static class PropertyFactory
    {
        public static IPropertyValue Construct(string component, string property, JToken value)
        {
            var cdef = Editor.ComponentDefinitionsMap[component];
            var prop = cdef.Properties.FirstOrDefault(p => p.Property == property);
            if (prop == null)
                return null;

            var pvalue = ConstructType(prop, value);
            if (pvalue != null)
            {
                pvalue.Locked = prop.Usage.Locked;
            }

            return pvalue;
        }

        public static IPropertyValue ConstructType(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            switch (prop.Usage.Type)
            {
                case "bool":
                    return ConstructBool(prop, value);

                case "float":
                    return ConstructFloat(prop, value);

                case "string":
                    return ConstructString(prop, value);

                case "array":
                    return ConstructArray(prop, value);

                case "vector":
                    return ConstructVector(prop, value);

                case "texture":
                    return ConstructTexture(prop, value);

                case "entity":
                    return ConstructEntity(prop, value);

                case "enum":
                    return ConstructEnum(prop, value);

                default:
                    return null;
            }
        }

        private static IPropertyValue ConstructBool(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default;
            bool? bvalue = null;

            if (prop.Usage.Required)
            {
                bvalue = false;
            }

            if (def != null && def.Type == JTokenType.Boolean)
            {
                bvalue = (bool) def;
            }

            if (value != null)
            {
                if (value.Type == JTokenType.Boolean)
                {
                    bvalue = (bool) value;
                }
                else if (!prop.Usage.Required && value.Type == JTokenType.Null)
                {
                    bvalue = null;
                }
            }

            return new Bool {Value = bvalue};
        }

        private static IPropertyValue ConstructFloat(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default;

            if (value == null)
                value = def;

            return new Float {Value = (float?) value};
        }

        private static IPropertyValue ConstructString(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default ?? "";

            if (value == null)
                value = def;

            return new String(prop.Usage.Semantics) {Value = (string) value};
        }

        private static IPropertyValue ConstructArray(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default ?? new JArray();

            if (value == null)
                value = def;

            return new Array((JArray) value, prop);
        }

        private static IPropertyValue ConstructVector(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var ary = value as JArray;
            var def = prop.Usage.Default as JArray;

            if (ary == null)
                ary = def;

            if (ary == null)
                ary = new JArray();

            return new Vector(ary, prop.Usage.VectorDimensions ?? 4, prop.Usage.Semantics);
        }

        private static IPropertyValue ConstructTexture(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default ?? "";

            if (value == null)
                value = def;

            return new Texture(value);
        }

        private static IPropertyValue ConstructEntity(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default ?? new JObject();

            if (value == null)
                value = def;

            return new Entity((JObject) value);
        }

        private static IPropertyValue ConstructEnum(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default ?? null;

            if (value == null)
                value = def;

            var options = new List<string>();
            if (!prop.Usage.Required)
            {
                options.Add("");
            }

            options.AddRange(prop.Usage.EnumOptions.Select(t => (string) t));

            return new Enum((string) value, options.ToArray());
        }
    }
}