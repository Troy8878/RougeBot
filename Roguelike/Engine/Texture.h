/*********************************
 * Texture.h
 * Connor Hilarides
 * Created 2014/08/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\FileSystem.h"

// ----------------------------------------------------------------------------

struct ImageResource;
class TextureZip;

// ----------------------------------------------------------------------------

mrb_value mrb_texture_init(mrb_state *mrb, const Texture2D &tex);

// ----------------------------------------------------------------------------

class Texture2D
{
public:
  Texture2D() = default;

  PROPERTY(get = _GetShaderRes) ID3D11ShaderResourceView * const &ShaderRes;
  PROPERTY(get = _GetTexture) ID3D11Texture2D * const &Texture;

  /**
    The render target for SPECIAL/SURFACE textures
  */
  PROPERTY(get = _GetRenderTarget) ID2D1Bitmap1 * const &RenderTarget;

  /**
    The zip for a zipped texture
  */
  PROPERTY(get = _GetTextureZip) TextureZip &Zip;

  ID2D1BitmapBrush1 *To2DBrush();

  operator bool() const
  {
    return !!_res;
  }

  static Texture2D GetNullTexture();
  static Texture2D CreateD2DSurface(GraphicsDevice *device, UINT width, UINT height);
  static Texture2D FromTextureZip(TextureZip &zip);

  PROPERTY(get = GetName) const std::string &Name;
  PROPERTY(get = GetWidth) size_t Width;
  PROPERTY(get = GetHeight) size_t Height;

  PROPERTY(get = GetRubyWrapper) mrb_value RubyWrapper;

  static Texture2D LoadTextureDefinition(json::value definition);

private:
  struct TextureResource;

  static Texture2D ConstructTexture(json::value definition);
  static Texture2D ConstructZipped(json::value definition);

  explicit Texture2D(const std::shared_ptr<TextureResource> &resource);
  Texture2D(ID3D11Device *device, const std::string &asset);
  Texture2D(ID3D11Device *device, ImageResource resource);

  struct TextureResource
  {
    typedef GraphicsDevice::D2DData::clock clock;

    std::string name;

    ID3D11Texture2D *texture = nullptr;
    ID3D11ShaderResourceView *resource = nullptr;

    // SPECIAL/SURFACE data
    IDXGISurface *surface = nullptr;
    ID2D1Bitmap1 *target = nullptr;
    ID2D1Bitmap1 *bitmap = nullptr;
    ID2D1BitmapBrush1 *brush = nullptr;
    GraphicsDevice *device = nullptr;
    UINT width, height;
    clock::time_point timestamp;
    clock::time_point brush_timestamp;
    TextureZip *zip;

    TextureResource() = default;
    ~TextureResource();

    NO_COPY_CONSTRUCTOR(TextureResource);
    NO_ASSIGNMENT_OPERATOR(TextureResource);

    // Move constructor
    TextureResource(TextureResource &&moving)
    {
      std::swap(texture, moving.texture);
      std::swap(resource, moving.resource);
      std::swap(surface, moving.surface);
      std::swap(target, moving.target);
      std::swap(device, moving.device);
      std::swap(width, moving.width);
      std::swap(height, moving.height);
      std::swap(timestamp, moving.timestamp);
    }

    void ValidateSpecialSurface();
  };

  std::shared_ptr<TextureResource> _res;

  friend mrb_value mrb_texture_init(mrb_state *mrb, const Texture2D &tex);

public:
  ID3D11ShaderResourceView * const &_GetShaderRes() const
  {
    return _res->resource;
  };

  ID3D11Texture2D * const &_GetTexture() const
  {
    return _res->texture;
  };

  ID2D1Bitmap1 * const &_GetRenderTarget() const
  {
    _res->ValidateSpecialSurface();
    return _res->target;
  }

  TextureZip &_GetTextureZip() const
  {
    return *_res->zip;
  }

  mrb_value GetRubyWrapper() const;

  const std::string &GetName() const
  {
    return _res->name;
  }

  size_t GetWidth() const
  {
    return _res->width;
  }

  size_t GetHeight() const
  {
    return _res->height;
  }

  // Non-const versions because ruby demands it :/
  const std::string &GetName()
  {
    return _res->name;
  }

  size_t GetWidth()
  {
    return _res->width;
  }

  size_t GetHeight()
  {
    return _res->height;
  }

  friend class TextureManager;
  friend class TextureZip;
  friend static mrb_value mrb_texture_save(mrb_state *mrb, mrb_value self);
};

// ----------------------------------------------------------------------------

class TextureManager
{
public:
  typedef std::unordered_map<std::string, std::weak_ptr<Texture2D::TextureResource>> map_type;

  static TextureManager Instance;

  Texture2D LoadTexture(const std::string &asset);
  bool IsTextureCached(const std::string &asset);

  map_type::iterator begin() { return _resources.begin(); }
  map_type::iterator end() { return _resources.end(); }

  NO_COPY_CONSTRUCTOR(TextureManager);
  NO_ASSIGNMENT_OPERATOR(TextureManager);

private:
  TextureManager();

  map_type _resources;
};

// ----------------------------------------------------------------------------