/*********************************
 * BasicPixelShader.hlsl
 * Connor Hilarides
 * Created 2014/08/20
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "ShaderCommons.hlsli"

float4 main(BasicPixelInputType input) : SV_TARGET
{
  return input.color;
}
