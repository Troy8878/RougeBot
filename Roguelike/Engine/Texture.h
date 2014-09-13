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

  /**
    The render target for SPECIAL/SURFACE textures
  */
  PROPERTY(get = _GetRenderTarget) ID2D1Bitmap1 * const & RenderTarget;

  operator bool() const { return !!_res; }

  static Texture2D GetNullTexture(ID3D11Device *device);
  static Texture2D CreateD2DSurface(GraphicsDevice *device, UINT width, UINT height);

private:
  struct TextureResource;
  
  explicit Texture2D(const std::shared_ptr<TextureResource>& resource);
  Texture2D(ID3D11Device *device, const std::string& asset);
  Texture2D(ID3D11Device *device, ImageResource resource);

  struct TextureResource
  {
    typedef GraphicsDevice::D2DData::clock clock;

    ID3D11Texture2D *texture = nullptr;
    ID3D11ShaderResourceView *resource = nullptr;

    // SPECIAL/SURFACE data
    IDXGISurface *surface = nullptr;
    ID2D1Bitmap1 *target = nullptr;
    GraphicsDevice *device = nullptr;
    UINT width, height;
    clock::time_point timestamp;

    TextureResource() = default;
    ~TextureResource();

    NO_COPY_CONSTRUCTOR(TextureResource);
    NO_ASSIGNMENT_OPERATOR(TextureResource);

    // Move constructor
    TextureResource(TextureResource&& moving)
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

public:
  ID3D11ShaderResourceView * const & _GetShaderRes() const { return _res->resource; };
  ID3D11Texture2D * const & _GetTexture() const { return _res->texture; };
  ID2D1Bitmap1 * const & _GetRenderTarget() const 
  { 
    _res->ValidateSpecialSurface(); 
    return _res->target; 
  }

  friend class TextureManager;
  friend class TextureZip;
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

