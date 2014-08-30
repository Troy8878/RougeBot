/*********************************
 * Shader.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"
#include "GraphicsDevice.h"

// ----------------------------------------------------------------------------

struct Camera;
struct Texture;

// ----------------------------------------------------------------------------

struct Shader
{
  GraphicsDevice *device;
  ID3D11VertexShader *vertexShader;
  ID3D11InputLayout *vertexLayout;
  ID3D11PixelShader *pixelShader;
  ID3D11Buffer *cameraBuffer;
  Camera *camera;
  Texture *texture = nullptr;

  shared_array<byte> vertexShaderData;
  shared_array<byte> pixelShaderData;

  virtual ~Shader();
  void Draw(unsigned indexCount);

  static void LoadShader(
    GraphicsDevice *device,
    const std::string& asset);

private:
  void InitCameraBuffer();
  static Shader *LoadShader(
    GraphicsDevice *device,
    const std::string& vertexAsset,
    const std::string& pixelAsset);
};

// ----------------------------------------------------------------------------

extern std::unordered_map<std::string, Shader *> RegisteredShaders;

// ----------------------------------------------------------------------------
