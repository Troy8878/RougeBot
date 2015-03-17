using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using EntityEditor.Entities.Representations.Properties;
using EntityEditor.Entities.Serialization;
using EntityEditor.Helpers;
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
            pvalue.Locked = prop.Usage.Locked;

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

                case "integer":
                    return ConstructInteger(prop, value);

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

                case "map":
                    return ConstructMap(prop, value);

                default:
                    return ConstructRaw(prop, value);
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
        
        private static IPropertyValue ConstructInteger(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default;

            if (value == null)
                value = def;

            return new Integer {Value = (long?) value};
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
            var def = prop.Usage.Default as JArray;

            if (value == null)
                value = def;

            if (value == null)
                value = new JArray();

            if (value.Type == JTokenType.String)
            {
                var str = (string) value;
                if (str.StartsWith("#"))
                {
                    value = str.TrimStart('#').HexToColor();
                }
                else
                {
                    var split = str.Split(',').Select(s => s.Trim()).ToArray();
                    var color = split[0].Trim().NamedColor();
                    if (split.Length > 1)
                        color.ScA = float.Parse(split[1].Trim());
                    value = new JArray(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
                }
            }

            return new Vector(value as JArray, prop.Usage.VectorDimensions ?? 4, prop.Usage.Semantics);
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
            var def = prop.Usage.Default ?? "";

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

        private static IPropertyValue ConstructMap(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            var def = prop.Usage.Default ?? new JObject();

            if (value == null)
                value = def;

            if (prop.Usage.MapKey == "string")
            {
                if (prop.Usage.MapValue == "string")
                {
                    return new StringMap((JObject) value);
                }
            }

            // TODO: Handle more map types (do we even have any?)
            return ConstructRaw(prop, value);
        }

        private static IPropertyValue ConstructRaw(ComponentDefinition.ComponentProperty prop, JToken value)
        {
            value = value ?? prop.Usage.Default;
            return new Raw {JsonValue = value};
        }
    }
}