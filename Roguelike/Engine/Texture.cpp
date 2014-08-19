/*********************************
 * Texture.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "Common.h"
#include "Texture.h"
#include "ResourceReader\ImageResource.h"
#include "Game.h"
#include "ResourceReader\ResourcePack.h"

// ----------------------------------------------------------------------------

Texture2D::Texture2D(ID3D11Device *device, const std::string& asset)
  : Texture2D{device, ImageResource::fromAsset("Textures", asset)}
{
}

// ----------------------------------------------------------------------------

Texture2D::Texture2D(ID3D11Device *device, ImageResource resource)
{
  shared_array<byte> imageData = resource.data;
  if (resource.format != ImageResource::Format::RGBA)
  {
    imageData = shared_array<byte>(resource.width * resource.height * 4);
    resource.to32BitColor(imageData);
  }

  ID3D11ShaderResourceView *res;
  ID3D11Texture2D *tex;

  D3D11_TEXTURE2D_DESC tdesc;
  D3D11_SUBRESOURCE_DATA tbsd;

  tbsd.pSysMem = imageData;
  tbsd.SysMemPitch = resource.width * 4;
  tbsd.SysMemSlicePitch = resource.width * resource.height * 4;

  tdesc.Width = resource.width;
  tdesc.Height = resource.height;
  tdesc.MipLevels = 1;
  tdesc.ArraySize = 1;

  tdesc.SampleDesc.Count = 1;
  tdesc.SampleDesc.Quality = 0;
  tdesc.Usage = D3D11_USAGE_DEFAULT;
  tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

  tdesc.CPUAccessFlags = 0;
  tdesc.MiscFlags = 0;

  auto result = device->CreateTexture2D(&tdesc, &tbsd, &tex);
  CHECK_HRESULT(result);

  D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
  sdesc.Format = tdesc.Format;
  sdesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
  sdesc.Texture2D.MipLevels = tdesc.MipLevels;
  sdesc.Texture2D.MostDetailedMip = tdesc.MipLevels - 1;

  result = device->CreateShaderResourceView(tex, &sdesc, &res);
  CHECK_HRESULT(result);

  _res = std::make_shared<TextureResource>();
  _res->texture = tex;
  _res->resource = res;
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::GetNullTexture(ID3D11Device *device)
{
  static bool initialized = false;
  static Texture2D nullTexture;
  if (initialized)
    return nullTexture;

  nullTexture = Texture2D{device, ImageResource{1, 1, ImageResource::Format::RGBA, {255, 255, 255, 255}}};
  initialized = true;
  return nullTexture;
}

// ----------------------------------------------------------------------------

Texture2D::TextureResource::~TextureResource()
{
  ReleaseDXInterface(resource);
  ReleaseDXInterface(texture);
}

// ----------------------------------------------------------------------------

