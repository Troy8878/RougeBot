/*********************************
 * Texture.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"

struct ImageResource;

class Texture2D
{
public:
  static std::string respack_container_name;

  Texture2D() = default;
  Texture2D(ID3D11Device *device, const std::wstring& file);
  Texture2D(ID3D11Device *device, ImageResource resource);

  ID3D11ShaderResourceView * const & getShaderRes() const { return _res->resource; };
  ID3D11Texture2D * const & getTexture() const { return _res->texture; };

  operator bool() const { return !!_res; }

  static Texture2D getNullTexture(ID3D11Device *device);

private:
  struct TextureResource
  {
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *resource;

    ~TextureResource();
  };

  std::shared_ptr<TextureResource> _res;
};

