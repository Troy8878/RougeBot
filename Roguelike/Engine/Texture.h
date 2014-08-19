/*********************************
 * Texture.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"

// ----------------------------------------------------------------------------

struct ImageResource;

// ----------------------------------------------------------------------------

class Texture2D
{
public:
  Texture2D() = default;
  Texture2D(ID3D11Device *device, const std::string& asset);
  Texture2D(ID3D11Device *device, ImageResource resource);

  __declspec(property(get = _GetShaderRes)) ID3D11ShaderResourceView * const & ShaderRes;
  __declspec(property(get = _GetTexture)) ID3D11Texture2D * const & Texture;

  operator bool() const { return !!_res; }

  static Texture2D GetNullTexture(ID3D11Device *device);

private:
  struct TextureResource
  {
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *resource;

    ~TextureResource();
  };

  std::shared_ptr<TextureResource> _res;

public:
  ID3D11ShaderResourceView * const & _GetShaderRes() const { return _res->resource; };
  ID3D11Texture2D * const & _GetTexture() const { return _res->texture; };
};

// ----------------------------------------------------------------------------

