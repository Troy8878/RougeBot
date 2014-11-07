/*********************************
 * ShaderCommons.hlsli
 * Connor Hilarides
 * Created 2014/08/20
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

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
  float4 tex : TEXCOORD;
};

struct TexturedPixelInputType
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float4 tex : TEXCOORD;
};
