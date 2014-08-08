#include "ShaderCommons.hlsli"

float4 main(BasicPixelInputType input) : SV_TARGET
{
  return input.color;
}
