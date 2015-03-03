/*********************************
 * Shader.h
 * Connor Hilarides
 * Created 2014/07/07
 * Copyright � 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"
#include "GraphicsDevice.h"

// ----------------------------------------------------------------------------

struct Camera;

// ----------------------------------------------------------------------------

struct Shader
{
  GraphicsDevice *device;
  ID3D11VertexShader *vertexShader;
  ID3D11InputLayout *vertexLayout;
  ID3D11PixelShader *pixelShader;
  ID3D11Buffer *cameraBuffer;
  Camera *camera;

  shared_array<byte> vertexShaderData;
  shared_array<byte> pixelShaderData;

  virtual ~Shader();
  void Draw(unsigned indexCount);

  static void LoadShader(
    GraphicsDevice *device,
    const std::string &asset);

private:
  void InitCameraBuffer();
  static Shader *LoadShader(
    GraphicsDevice *device,
    const std::string &vertexAsset,
    const std::string &pixelAsset);
};

// ----------------------------------------------------------------------------

struct ShaderManager
{
  std::unordered_map<std::string, Shader *> shaders;

  Shader *&operator[](const std::string &key)
  {
    if (key == "")
      return shaders["Textured"];
    return shaders[key];
  }
};

extern ShaderManager RegisteredShaders;

// ----------------------------------------------------------------------------