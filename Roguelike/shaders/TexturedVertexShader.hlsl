#include "ShaderCommons.hlsli"

cbuffer CameraData
{
  float4x4 worldMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
};

TexturedPixelInputType main(TexturedVertexInputType input)
{
  TexturedPixelInputType output;
  
  output.position = float4(input.position.xyz, 1);
  
  output.position = mul(output.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);

  output.color = input.color;
  output.tex = input.tex;

  return output;
}
