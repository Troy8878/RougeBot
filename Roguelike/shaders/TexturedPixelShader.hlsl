/*********************************
 * TexturedPixelShader.hlsl
 * Connor Hilarides
 * Created 2014/08/20
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

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
