#include "ShaderCommons.hlsli"

cbuffer CameraData
{
  float4x4 worldMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
};

cbuffer VertexTime
{
  float time;
};

TexturedPixelInputType main(TexturedVertexInputType input)
{
  TexturedPixelInputType output;
  
  output.position = float4(input.position.xyz, 1);
  output.tex = input.tex;
  output.color = input.color;
  
  // Object to World space
  output.position = mul(output.position, worldMatrix);
  // World to Camera Space
  output.position = mul(output.position, viewMatrix);
  // Do some funky shakey shit
  output.position.z += sin(output.position.x / 2 + time * 2) / 3;
  // Light thingy
  float dist = (distance(output.position.xy, float2(0, 0)) + 1.9) / 2;
  output.color.xyz *= 1 / dist;
  // Camera to Projection space
  output.position = mul(output.position, projectionMatrix);

  return output;
}
