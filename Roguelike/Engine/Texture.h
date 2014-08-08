/*********************************
 * Texture.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"

struct ImageResource;

class Texture
{
public:
  Texture(ID3D11Device *device, const fs::wpath& file);
  Texture(ID3D11Device *device, ImageResource resource);

  ID3D11ShaderResourceView *getTexture();

private:
  struct TextureResource
  {
    ID3D11ShaderResourceView *texture;

    ~TextureResource();
  };

  std::shared_ptr<TextureResource> resource;
};

