#include "ShaderCommons.hlsli"

cbuffer ColorData
{
  float4 tint;
};

Texture2D shaderTexture;
Texture2D tintTexture;
SamplerState sampleState;
SamplerState sampleTint;

float4 main(TexturedPixelInputType input) : SV_TARGET
{
  float4 texColor = float4(1, 1, 1, 1);

  texColor *= shaderTexture.Sample(sampleState, input.tex.xy);
  texColor *= tintTexture.Sample(sampleTint, input.tex.xy);
  texColor *= input.color;
  texColor *= tint;

  return texColor;
}
