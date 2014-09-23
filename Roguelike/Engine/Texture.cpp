/*********************************
 * Texture.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "Common.h"
#include "ResourceReader\ImageResource.h"
#include "ResourceReader\ResourcePack.h"
#include "Game.h"
#include "Texture.h"
#include "TextureZip.h"

// ----------------------------------------------------------------------------

TextureManager TextureManager::Instance;

// ----------------------------------------------------------------------------

Texture2D::Texture2D(const std::shared_ptr<TextureResource>& resource)
  : _res(resource)
{
}

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
  tdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

  nullTexture = Texture2D
  {
    device, 
    ImageResource
    {
      1, 1,
      ImageResource::Format::RGBA,
      {255, 255, 255, 255}
    }
  };

  initialized = true;
  return nullTexture;
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::CreateD2DSurface(GraphicsDevice *device, UINT width, UINT height)
{
  TextureResource resource;

  resource.device = device;
  resource.width = width;
  resource.height = height;
  resource.ValidateSpecialSurface();

  return Texture2D{std::make_shared<TextureResource>(std::move(resource))};
}

// ----------------------------------------------------------------------------

Texture2D::TextureResource::~TextureResource()
{
  ReleaseDXInterface(target);
  ReleaseDXInterface(surface);
  ReleaseDXInterface(resource);
  ReleaseDXInterface(texture);
}

// ----------------------------------------------------------------------------

TextureManager::TextureManager()
{
}

// ----------------------------------------------------------------------------

Texture2D TextureManager::LoadTexture(const std::string& asset)
{
  auto it = _resources.find(asset);
  if (it != _resources.end())
  {
    auto weak_ref = it->second;
    auto strong_ref = weak_ref.lock();
    if (strong_ref)
    {
      return Texture2D{strong_ref};
    }
    else
    {
      _resources.erase(it);
    }
  }

  auto device = GetGame()->GameDevice;

  Texture2D texture;
  if (asset.find("SPECIAL/SURFACE/") == 0)
  {
    std::string data{asset.begin() + asset.find_first_not_of("SPECIAL/SURFACE/"), asset.end()};
    auto cpos = data.find(':'); (cpos);
    assert(cpos + 1 < data.size());

    UINT width = std::stoul(std::string{data.begin(), data.begin() + cpos});
    UINT height = std::stoul(std::string{data.begin() + cpos + 1, data.end()});

    texture = Texture2D::CreateD2DSurface(device, width, height);
    // These are to be unique, don't cache them
  }
  else
  {
    texture = Texture2D{device->Device, asset};
    _resources[asset] = texture._res;
  }

  return texture;
}

// ----------------------------------------------------------------------------

bool TextureManager::IsTextureCached(const std::string& asset)
{
  auto it = _resources.find(asset);
  return it != _resources.end() && !it->second.expired();
}

// ----------------------------------------------------------------------------

void Texture2D::TextureResource::ValidateSpecialSurface()
{
  if (!device)
    throw std::exception("I think someone forgot to use a special surface for text drawing");

  if (timestamp >= device->D2D.ResourceTimestamp)
    return;

  HRESULT hr;

  #pragma region Texture

  D3D11_TEXTURE2D_DESC td;
  td.ArraySize = 1;
  td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  td.CPUAccessFlags = 0;
  td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  td.Height = height;
  td.Width = width;
  td.MipLevels = 1;
  td.MiscFlags = 0;
  td.SampleDesc.Count = 1;
  td.SampleDesc.Quality = 0;
  td.Usage = D3D11_USAGE_DEFAULT;

  hr = device->Device->CreateTexture2D(&td, nullptr, &texture);
  CHECK_HRESULT(hr);

  #pragma endregion

  #pragma region Shader Resource

  D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
  sdesc.Format = td.Format;
  sdesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
  sdesc.Texture2D.MipLevels = td.MipLevels;
  sdesc.Texture2D.MostDetailedMip = td.MipLevels - 1;

  hr = device->Device->CreateShaderResourceView(texture, &sdesc, &resource);
  CHECK_HRESULT(hr);

  #pragma endregion

  #pragma region D2D Resource

  hr = texture->QueryInterface(&surface);
  CHECK_HRESULT(hr);

  D2D1_BITMAP_PROPERTIES1 props =
    D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_TARGET, 
      D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 
      96, 96);

  hr = device->D2D.DeviceContext->CreateBitmapFromDxgiSurface(
    surface,
    &props,
    &target);
  CHECK_HRESULT(hr);

  timestamp = clock::now();

  #pragma endregion
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::FromTextureZip(TextureZip& zip)
{
  // Just make a new Zip and point it to itself
  auto pZip = new TextureZip(zip);
  pZip->Texture._res->zip = pZip;
  return pZip->Texture;
}

// ----------------------------------------------------------------------------

