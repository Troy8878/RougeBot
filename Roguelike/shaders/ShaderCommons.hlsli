
struct BasicVertexInputType
{
  float4 position : POSITION;
  float4 color : COLOR;
};

struct BasicPixelInputType
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

struct TexturedVertexInputType
{
  float4 position : POSITION;
  float4 color : COLOR;
  float2 tex : TEXCOORD0;
};

struct TexturedPixelInputType
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float2 tex : TEXCOORD0;
};
