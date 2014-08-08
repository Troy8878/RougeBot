#include "ShaderCommons.hlsli"

Texture2D shaderTexture;
SamplerState sampleType;

float4 main(TexturedPixelInputType input) : SV_TARGET
{
  float4 texColor;

  texColor = shaderTexture.Sample(sampleType, input.tex);
  texColor = texColor * input.color;

  return texColor;
}
