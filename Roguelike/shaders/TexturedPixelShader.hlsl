#include "ShaderCommons.hlsli"

cbuffer ColorData
{
  float4 tint;
};

Texture2D shaderTexture;
SamplerState sampleState;

float4 main(TexturedPixelInputType input) : SV_TARGET
{
  float4 texColor;

  texColor = shaderTexture.Sample(sampleState, input.tex.xy);
  texColor = texColor * input.color * tint;

  return texColor;
}
