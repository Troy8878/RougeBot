using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Entities.Serialization
{
    public class ComponentDefinition
    {
        [JsonProperty("name")]
        public string Name { get; set; }

        [JsonProperty("dependencies")]
        public string[] Dependencies { get; set; }

        [JsonProperty("properties")]
        public ComponentProperty[] Properties { get; set; }

        public class ComponentProperty
        {
            [JsonProperty("property")]
            public string Property { get; set; }

            [JsonProperty("usage")]
            public PropertyUsage Usage { get; set; }

            public class PropertyUsage
            {
                [JsonProperty("type")]
                public string Type { get; set; }

                [JsonProperty("required")]
                public bool Required { get; set; }

                [JsonProperty("semantics")]
                public string Semantics { get; set; }

                [JsonProperty("locked")]
                public bool Locked { get; set; }

                [JsonProperty("default")]
                public JToken Default { get; set; }

                [JsonProperty("options")]
                public JToken[] EnumOptions { get; set; }

                [JsonProperty("dimensions")]
                public int? VectorDimensions { get; set; }

                [JsonProperty("contained")]
                public string ArrayContains { get; set; }

                [JsonProperty("key")]
                public string MapKey { get; set; }

                [JsonProperty("value")]
                public string MapValue { get; set; }
            }
        }
    }
}
