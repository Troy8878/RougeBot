/*********************************
 * BasicVertexShader.hlsl
 * Connor Hilarides
 * Created 2014/08/20
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "ShaderCommons.hlsli"

cbuffer CameraData
{
  float4x4 worldMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
};

BasicPixelInputType main(BasicVertexInputType input)
{
  BasicPixelInputType output;
  
  output.position = float4(input.position.xyz, 1);
  
  output.position = mul(output.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);

  output.color = input.color;

  return output;
}
