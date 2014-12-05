/*********************************
 * Texture.cpp
 * Connor Hilarides
 * Created 2014/08/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

Texture2D::Texture2D(const std::shared_ptr<TextureResource> &resource)
  : _res(resource)
{
}

// ----------------------------------------------------------------------------

Texture2D::Texture2D(ID3D11Device *device, const std::string &asset)
  : Texture2D{device, ImageResource::fromAsset("Textures", asset)}
{
}

// ----------------------------------------------------------------------------

Texture2D::Texture2D(ID3D11Device *device, ImageResource resource)
{
  shared_array<byte> imageData = resource.data;
  if (resource.format != ImageResource::Format::BGRA)
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
  _res->width = resource.width;
  _res->height = resource.height;
  _res->device = GetGame()->GameDevice;
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::GetNullTexture()
{
  static bool initialized = false;
  static Texture2D nullTexture;

  if (initialized)
    return nullTexture;

  nullTexture = Texture2D
    {
      GetGame()->GameDevice->Device,
      ImageResource
      {
        1, 1,
        ImageResource::Format::RGBA,
        {255, 255, 255, 255}
      }
    };

  nullTexture._res->name = "SPECIAL/NULL";

  initialized = true;
  return nullTexture;
}

// ----------------------------------------------------------------------------

ID2D1BitmapBrush1 *Texture2D::To2DBrush()
{
  if (!_res->device)
    return nullptr;

  HRESULT hr;
  auto &d2d = _res->device->D2D;

  if (_res->brush && _res->brush_timestamp >= d2d.ResourceTimestamp)
    return _res->brush;

  if (!_res->surface)
  {
    hr = _res->texture->QueryInterface(&_res->surface);
    CHECK_HRESULT(hr);
  }

  D2D1_BITMAP_PROPERTIES1 props =
    D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_NONE,
      D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
      96, 96);

  ReleaseDXInterface(_res->bitmap);
  hr = d2d.DeviceContext->CreateBitmapFromDxgiSurface(
    _res->surface,
    &props,
    &_res->bitmap);
  CHECK_HRESULT(hr);

  ReleaseDXInterface(_res->brush);
  hr = d2d.DeviceContext->CreateBitmapBrush(_res->target, &_res->brush);
  CHECK_HRESULT(hr);

  _res->brush_timestamp = TextureResource::clock::now();
  return _res->brush;
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
  ReleaseDXInterface(brush);
  ReleaseDXInterface(bitmap);
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

Texture2D TextureManager::LoadTexture(const std::string &asset)
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

    _resources.erase(it);
  }

  auto device = GetGame()->GameDevice;

  Texture2D texture;
  if (asset == "SPECIAL/NULL")
  {
    texture = Texture2D::GetNullTexture();
  }
  else if (asset.find("SPECIAL/SURFACE/") == 0)
  {
    std::string data{asset.begin() + asset.find_first_not_of("SPECIAL/SURFACE/"), asset.end()};
    auto cpos = data.find(':');
    (cpos);
    assert(cpos + 1 < data.size());

    UINT width = std::stoul(std::string{data.begin(), data.begin() + cpos});
    UINT height = std::stoul(std::string{data.begin() + cpos + 1, data.end()});

    texture = Texture2D::CreateD2DSurface(device, width, height);
    texture._res->name = asset;
    // These are to be unique, don't cache them
  }
  else if (asset.find("SPECIAL/SHARED_SURFACE/") == 0)
  {
    std::string data{asset.begin() + asset.find_first_not_of("SPECIAL/SHARED_SURFACE/"), asset.end()};
    auto cpos = data.find(':');
    assert(cpos + 1 < data.size());
    auto clast = data.find_first_of('/', cpos + 1);

    UINT width = std::stoul(std::string{data.begin(), data.begin() + cpos});
    UINT height = std::stoul(std::string{data.begin() + cpos + 1, data.begin() + clast});

    texture = Texture2D::CreateD2DSurface(device, width, height);
    texture._res->name = asset;
    _resources[asset] = texture._res;
  }
  else
  {
    texture = Texture2D{device->Device, asset};
    texture._res->name = asset;
    _resources[asset] = texture._res;
  }

  return texture;
}

// ----------------------------------------------------------------------------

bool TextureManager::IsTextureCached(const std::string &asset)
{
  auto it = _resources.find(asset);
  return it != _resources.end() && !it->second.expired();
}

// ----------------------------------------------------------------------------

void Texture2D::TextureResource::ValidateSpecialSurface()
{
  if (!device)
    throw basic_exception("I think someone forgot to use a special surface for text drawing");

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

Texture2D Texture2D::FromTextureZip(TextureZip &zip)
{
  // Just make a new Zip and point it to itself
  auto pZip = new TextureZip(zip);
  pZip->Texture._res->zip = pZip;
  return pZip->Texture;
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::LoadTextureDefinition(json::value definition)
{
  static std::unordered_map<std::string, Texture2D(*)(json::value)> constructors =
    {
      {"texture", Texture2D::ConstructTexture},
      {"zipped", Texture2D::ConstructZipped}
    };

  if (definition.is(json::json_type::jobject))
  {
    auto &obj = definition.as_object();

    assert(obj.size() == 1); // THERE CAN ONLY BE ONE!!1!
    auto &pair = *obj.begin();

    return constructors[pair.first](pair.second);
  }

  // A single string must mean a single texture
  assert(definition.is(json::json_type::jstring));
  return ConstructTexture(definition);
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::ConstructTexture(json::value definition)
{
  return TextureManager::Instance.LoadTexture(definition.as_string());
}

// ----------------------------------------------------------------------------

Texture2D Texture2D::ConstructZipped(json::value definition)
{
  assert(definition.is_array_of<json::value::string_t>());

  TextureZip zip(definition.as_array_of<json::value::string_t>());
  return Texture2D::FromTextureZip(zip);
}

// ----------------------------------------------------------------------------

mrb_data_type mrb_texture_2d_dt;

mrb_value mrb_texture_init(mrb_state *mrb, const Texture2D &tex);

static mrb_value mrb_texture_load(mrb_state *mrb, mrb_value self);
static mrb_value mrb_texture_save(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

mrb_value Texture2D::GetRubyWrapper() const
{
  return mrb_texture_init(*mrb_inst, *this);
}

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_texture_init(mrb_state *mrb)
{
  mrb_texture_2d_dt.dfree = ruby::data_scalar_delete<Texture2D>;
  mrb_texture_2d_dt.struct_name = typeid(Texture2D).name();

  auto tex = mrb_define_class(mrb, "Texture", mrb->object_class);

  mrb_define_class_method(mrb, tex, "new", mrb_texture_load, ARGS_REQ(1));
  mrb_define_class_method(mrb, tex, "load", mrb_texture_load, ARGS_REQ(1));

  mrb_func_t get_name =
    ruby::data_getter_access_string<
      Texture2D, &mrb_texture_2d_dt,
      const std::string&, &Texture2D::GetName>;

  mrb_func_t get_width =
    ruby::data_getter_access_integer<
      Texture2D, &mrb_texture_2d_dt,
      size_t, &Texture2D::GetWidth>;

  mrb_func_t get_height =
    ruby::data_getter_access_integer<
      Texture2D, &mrb_texture_2d_dt,
      size_t, &Texture2D::GetHeight>;

  mrb_define_method(mrb, tex, "name", get_name, MRB_ARGS_NONE());
  mrb_define_method(mrb, tex, "width", get_width, MRB_ARGS_NONE());
  mrb_define_method(mrb, tex, "height", get_height, MRB_ARGS_NONE());

  mrb_define_method(mrb, tex, "save", mrb_texture_save, MRB_ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

mrb_value mrb_texture_init(mrb_state *mrb, const Texture2D &tex)
{
  static auto tex_c = mrb_class_get(mrb, "Texture");

  Texture2D *pTex;
  if (tex._res)
    pTex = new Texture2D(tex);
  else
    pTex = new Texture2D(Texture2D::GetNullTexture());

  auto obj = mrb_data_object_alloc(mrb, tex_c, pTex, &mrb_texture_2d_dt);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_texture_load(mrb_state *mrb, mrb_value)
{
  mrb_value opts;
  mrb_get_args(mrb, "o", &opts);

  auto jopts = mrb_inst->value_to_json(opts);
  auto tex = Texture2D::LoadTextureDefinition(jopts);

  return tex.RubyWrapper;
}

// ----------------------------------------------------------------------------

static IWICImagingFactory *GetWICFactory()
{
  THREAD_EXCLUSIVE_SCOPE;

  static IWICImagingFactory *factory = nullptr;
  if (factory)
    return factory;
  
  HRESULT hr = CoCreateInstance(
    CLSID_WICImagingFactory,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory,
    reinterpret_cast<LPVOID*>(&factory));
  CHECK_HRESULT(hr);

  return factory;
}

// ----------------------------------------------------------------------------

UINT nearestPO2(UINT value)
{
  UINT po2 = 1;
  while (po2 < value)
    po2 *= 2;
  return po2;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_texture_save(mrb_state *mrb, mrb_value self)
{
  auto tex = ruby::data_get<Texture2D>(mrb, self);

  mrb_value mrbfilename;
  mrb_get_args(mrb, "S", &mrbfilename);
  auto filename = widen(mrb_str_to_stdstring(mrbfilename));
  
  static auto *factory = GetWICFactory();
  HRESULT hr;

  IWICStream *stream = nullptr;
  hr = factory->CreateStream(&stream);
  CHECK_HRESULT(hr);

  hr = stream->InitializeFromFilename(filename.c_str(), GENERIC_WRITE);
  CHECK_HRESULT(hr);

  IWICBitmapEncoder *encoder = nullptr;
  hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
  CHECK_HRESULT(hr);

  hr = encoder->Initialize(stream, WICBitmapEncoderNoCache);
  CHECK_HRESULT(hr);

  IWICBitmapFrameEncode *frame = nullptr;
  IPropertyBag2 *propbag = nullptr;
  hr = encoder->CreateNewFrame(&frame, &propbag);
  CHECK_HRESULT(hr);

  hr = frame->Initialize(propbag);
  CHECK_HRESULT(hr);

  const UINT width = tex->_res->width;
  const UINT height = tex->_res->height;

  hr = frame->SetSize(width, height);
  CHECK_HRESULT(hr);

  WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
  hr = frame->SetPixelFormat(&formatGUID);
  CHECK_HRESULT(hr);
  
  #pragma region Setup Copy Texture

  auto graphics = GetGame()->GameDevice;

  ID3D11Texture2D *newTexture = nullptr;
  D3D11_TEXTURE2D_DESC newdesc;
  tex->Texture->GetDesc(&newdesc);
  newdesc.BindFlags = 0;
  newdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  newdesc.Usage = D3D11_USAGE_STAGING;

  hr = graphics->Device->CreateTexture2D(&newdesc, nullptr, &newTexture);
  CHECK_HRESULT(hr);
  
  graphics->DeviceContext->CopyResource(newTexture, tex->Texture);

  D3D11_MAPPED_SUBRESOURCE subres;
  hr = graphics->DeviceContext->Map(newTexture, 0, D3D11_MAP_READ, 0, &subres);
  CHECK_HRESULT(hr);

  #pragma endregion

  UINT stride = subres.RowPitch;
  UINT bufferSize = stride * height;
  hr = frame->WritePixels(height, stride, bufferSize,
                          reinterpret_cast<BYTE *>(subres.pData));
  CHECK_HRESULT(hr);

  hr = frame->Commit();
  CHECK_HRESULT(hr);

  hr = encoder->Commit();
  CHECK_HRESULT(hr);

  frame->Release();
  encoder->Release();
  stream->Release();

  graphics->DeviceContext->Unmap(newTexture, 0);
  newTexture->Release();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------