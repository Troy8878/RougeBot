/*********************************
 * Shader.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"
#include "GraphicsDevice.h"

struct Camera;
struct Texture;

struct Shader
{
  GraphicsDevice *device;
  ID3D11VertexShader *vertexShader;
  ID3D11InputLayout *vertexLayout;
  ID3D11PixelShader *pixelShader;
  ID3D11Buffer *cameraBuffer;
  Camera *camera;
  Texture *texture = nullptr;

  fs::binary_file_data vertexShaderData;
  fs::binary_file_data pixelShaderData;

  virtual ~Shader();
  void draw(unsigned indexCount);

  static Shader *loadShader(
    GraphicsDevice *device,
    const fs::wpath& vertexfile,
    const fs::wpath& pixelfile);

  void initCameraBuffer();
  void initializeBasicShader();
  void initializeTexturedShader();
};
