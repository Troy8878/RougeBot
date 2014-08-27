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

  PROPERTY(get = _GetShaderRes) ID3D11ShaderResourceView * const & ShaderRes;
  PROPERTY(get = _GetTexture) ID3D11Texture2D * const & Texture;

  operator bool() const { return !!_res; }

  static Texture2D GetNullTexture(ID3D11Device *device);

private:
  struct TextureResource;
  
  explicit Texture2D(const std::shared_ptr<TextureResource>& resource);
  Texture2D(ID3D11Device *device, const std::string& asset);
  Texture2D(ID3D11Device *device, ImageResource resource);

  struct TextureResource
  {
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *resource;

    TextureResource() = default;
    ~TextureResource();

    NO_COPY_CONSTRUCTOR(TextureResource);
    NO_ASSIGNMENT_OPERATOR(TextureResource);
  };

  std::shared_ptr<TextureResource> _res;

public:
  ID3D11ShaderResourceView * const & _GetShaderRes() const { return _res->resource; };
  ID3D11Texture2D * const & _GetTexture() const { return _res->texture; };

  friend class TextureManager;
};

// ----------------------------------------------------------------------------

class TextureManager
{
public:
  typedef std::unordered_map<std::string, std::weak_ptr<Texture2D::TextureResource>> map_type;

  static TextureManager Instance;

  Texture2D LoadTexture(const std::string& asset);
  bool IsTextureCached(const std::string& asset);

  map_type::iterator begin();
  map_type::iterator end();

  NO_COPY_CONSTRUCTOR(TextureManager);
  NO_ASSIGNMENT_OPERATOR(TextureManager);

private:
  TextureManager();

  map_type _resources;
};

// ----------------------------------------------------------------------------

