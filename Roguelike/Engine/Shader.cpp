/*********************************
 * Shader.cpp
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Game.h"
#include "json/json.h"

// ----------------------------------------------------------------------------

Shader::~Shader()
{
  ReleaseDXInterface(vertexShader);
  ReleaseDXInterface(pixelShader);
  ReleaseDXInterface(cameraBuffer);
}

// ----------------------------------------------------------------------------

DXGI_FORMAT ParseDXGIFormat(const std::string &fname)
{
#pragma region DXGI Formats, a long list :P
  static const std::unordered_map<std::string, UINT> formats
    {
      {"UNKNOWN", 0},
      {"R32G32B32A32_TYPELESS", 1},
      {"R32G32B32A32_FLOAT", 2},
      {"R32G32B32A32_UINT", 3},
      {"R32G32B32A32_SINT", 4},
      {"R32G32B32_TYPELESS", 5},
      {"R32G32B32_FLOAT", 6},
      {"R32G32B32_UINT", 7},
      {"R32G32B32_SINT", 8},
      {"R16G16B16A16_TYPELESS", 9},
      {"R16G16B16A16_FLOAT", 10},
      {"R16G16B16A16_UNORM", 11},
      {"R16G16B16A16_UINT", 12},
      {"R16G16B16A16_SNORM", 13},
      {"R16G16B16A16_SINT", 14},
      {"R32G32_TYPELESS", 15},
      {"R32G32_FLOAT", 16},
      {"R32G32_UINT", 17},
      {"R32G32_SINT", 18},
      {"R32G8X24_TYPELESS", 19},
      {"D32_FLOAT_S8X24_UINT", 20},
      {"R32_FLOAT_X8X24_TYPELESS", 21},
      {"X32_TYPELESS_G8X24_UINT", 22},
      {"R10G10B10A2_TYPELESS", 23},
      {"R10G10B10A2_UNORM", 24},
      {"R10G10B10A2_UINT", 25},
      {"R11G11B10_FLOAT", 26},
      {"R8G8B8A8_TYPELESS", 27},
      {"R8G8B8A8_UNORM", 28},
      {"R8G8B8A8_UNORM_SRGB", 29},
      {"R8G8B8A8_UINT", 30},
      {"R8G8B8A8_SNORM", 31},
      {"R8G8B8A8_SINT", 32},
      {"R16G16_TYPELESS", 33},
      {"R16G16_FLOAT", 34},
      {"R16G16_UNORM", 35},
      {"R16G16_UINT", 36},
      {"R16G16_SNORM", 37},
      {"R16G16_SINT", 38},
      {"R32_TYPELESS", 39},
      {"D32_FLOAT", 40},
      {"R32_FLOAT", 41},
      {"R32_UINT", 42},
      {"R32_SINT", 43},
      {"R24G8_TYPELESS", 44},
      {"D24_UNORM_S8_UINT", 45},
      {"R24_UNORM_X8_TYPELESS", 46},
      {"X24_TYPELESS_G8_UINT", 47},
      {"R8G8_TYPELESS", 48},
      {"R8G8_UNORM", 49},
      {"R8G8_UINT", 50},
      {"R8G8_SNORM", 51},
      {"R8G8_SINT", 52},
      {"R16_TYPELESS", 53},
      {"R16_FLOAT", 54},
      {"D16_UNORM", 55},
      {"R16_UNORM", 56},
      {"R16_UINT", 57},
      {"R16_SNORM", 58},
      {"R16_SINT", 59},
      {"R8_TYPELESS", 60},
      {"R8_UNORM", 61},
      {"R8_UINT", 62},
      {"R8_SNORM", 63},
      {"R8_SINT", 64},
      {"A8_UNORM", 65},
      {"R1_UNORM", 66},
      {"R9G9B9E5_SHAREDEXP", 67},
      {"R8G8_B8G8_UNORM", 68},
      {"G8R8_G8B8_UNORM", 69},
      {"BC1_TYPELESS", 70},
      {"BC1_UNORM", 71},
      {"BC1_UNORM_SRGB", 72},
      {"BC2_TYPELESS", 73},
      {"BC2_UNORM", 74},
      {"BC2_UNORM_SRGB", 75},
      {"BC3_TYPELESS", 76},
      {"BC3_UNORM", 77},
      {"BC3_UNORM_SRGB", 78},
      {"BC4_TYPELESS", 79},
      {"BC4_UNORM", 80},
      {"BC4_SNORM", 81},
      {"BC5_TYPELESS", 82},
      {"BC5_UNORM", 83},
      {"BC5_SNORM", 84},
      {"B5G6R5_UNORM", 85},
      {"B5G5R5A1_UNORM", 86},
      {"B8G8R8A8_UNORM", 87},
      {"B8G8R8X8_UNORM", 88},
      {"R10G10B10_XR_BIAS_A2_UNORM", 89},
      {"B8G8R8A8_TYPELESS", 90},
      {"B8G8R8A8_UNORM_SRGB", 91},
      {"B8G8R8X8_TYPELESS", 92},
      {"B8G8R8X8_UNORM_SRGB", 93},
      {"BC6H_TYPELESS", 94},
      {"BC6H_UF16", 95},
      {"BC6H_SF16", 96},
      {"BC7_TYPELESS", 97},
      {"BC7_UNORM", 98},
      {"BC7_UNORM_SRGB", 99},
      {"AYUV", 100},
      {"Y410", 101},
      {"Y416", 102},
      {"NV12", 103},
      {"P010", 104},
      {"P016", 105},
      {"420_OPAQUE", 106},
      {"YUY2", 107},
      {"Y210", 108},
      {"Y216", 109},
      {"NV11", 110},
      {"AI44", 111},
      {"IA44", 112},
      {"P8", 113},
      {"A8P8", 114},
      {"B4G4R4A4_UNORM", 115},
      {"FORCE_UINT", 0xffffffff}
    };
#pragma endregion

  auto it = formats.find(fname);
  if (it == formats.end())
    throw string_exception("Unknown DXGI Format '" + fname + "'");

  return static_cast<DXGI_FORMAT>(it->second);
}

// ----------------------------------------------------------------------------

void Shader::LoadShader(
  GraphicsDevice *device,
  const std::string &asset)
{
  auto container = GetGame()->Respack["Shaders"];
  RELEASE_AFTER_SCOPE(container);

  auto sdef_res = container->GetResource(asset);
  RELEASE_AFTER_SCOPE(sdef_res);

  auto deftree = json::value::parse(sdef_res->Stream).as_object();

  auto shader = LoadShader(device,
                           deftree["vertexShader"].as_string(),
                           deftree["pixelShader"].as_string());
  shader->InitCameraBuffer();

  auto layout = deftree["inputLayout"].as_array();
  assert(layout.size() <= 128);

  static json::value jzero = json::value::number(0);
  static json::value jtrue = json::value::boolean(true);

  D3D11_INPUT_ELEMENT_DESC inputs[128];
  for (int i = 0; i < layout.size(); ++i)
  {
    auto &layout_item = layout[i].as_object();

    inputs[i].SemanticName = layout_item["semantic"].as_string().c_str();
    inputs[i].SemanticIndex =
      static_cast<UINT>(map_fetch(layout_item, "semanticIndex", jzero).as_number());

    inputs[i].Format = ParseDXGIFormat(layout_item["format"].as_string());

    inputs[i].InputSlot =
      static_cast<UINT>(map_fetch(layout_item, "inputSlot", jzero).as_number());

    auto alignment = map_fetch(layout_item, "alignment", jzero);
    if (alignment.is(json::json_type::jstring))
    {
      assert(alignment.as_string() == "APPEND");
      inputs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    }
    else
    {
      inputs[i].AlignedByteOffset = static_cast<UINT>(alignment.as_number());
    }

    inputs[i].InputSlotClass =
      map_fetch(layout_item, "slotPerVertex", jtrue).as_bool()
        ? D3D11_INPUT_PER_VERTEX_DATA
        : D3D11_INPUT_PER_INSTANCE_DATA;

    inputs[i].InstanceDataStepRate =
      static_cast<UINT>(map_fetch(layout_item, "dataStepRate", jzero).as_number());
  }

  auto hr = device->Device->
                  CreateInputLayout(inputs, static_cast<UINT>(layout.size()),
                                    shader->vertexShaderData,
                                    shader->vertexShaderData.size(),
                                    &shader->vertexLayout);
  CHECK_HRESULT(hr);

  RegisteredShaders[deftree["name"].as_string()] = shader;
}

// ----------------------------------------------------------------------------

Shader *Shader::LoadShader(
  GraphicsDevice *device,
  const std::string &vertexAsset,
  const std::string &pixelAsset)
{
  HRESULT result;
  auto *shader = new Shader;
  shader->device = device;

  auto &respack = GetGame()->Respack;
  auto *shadersContainer = respack["Shaders"];
  RELEASE_AFTER_SCOPE(shadersContainer);

  auto *vertexRes = shadersContainer->GetResource(vertexAsset);
  RELEASE_AFTER_SCOPE(vertexRes);

  auto pixelRes = shadersContainer->GetResource(pixelAsset);
  RELEASE_AFTER_SCOPE(pixelRes);

  shader->vertexShaderData = shared_array<byte>{vertexRes->Data, vertexRes->Size};
  shader->pixelShaderData = shared_array<byte>{pixelRes->Data, pixelRes->Size};

  assert(shader->vertexShaderData);
  assert(shader->pixelShaderData);

  result = device->Device->CreateVertexShader(
                   shader->vertexShaderData,
                   shader->vertexShaderData.size(),
                   nullptr, &shader->vertexShader);
  CHECK_HRESULT(result);

  setDXDebugName(shader->vertexShader,
                 L"VertexShader{" + widen(vertexAsset) + L"}");

  result = device->Device->CreatePixelShader(
                   shader->pixelShaderData,
                   shader->pixelShaderData.size(),
                   nullptr, &shader->pixelShader);
  CHECK_HRESULT(result);

  setDXDebugName(shader->pixelShader,
                 L"PixelShader{" + widen(pixelAsset) + L"}");

  return shader;
}

// ----------------------------------------------------------------------------

static ID3D11Buffer *CreateTimeResource()
{
  D3D11_BUFFER_DESC desc;
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.ByteWidth = 16;
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  desc.MiscFlags = 0;
  desc.StructureByteStride = 0;

  ID3D11Buffer *buffer;

  HRESULT hr = GetGame()->GameDevice->Device->
                        CreateBuffer(&desc, nullptr, &buffer);
  CHECK_HRESULT(hr);

  return buffer;
}

// ----------------------------------------------------------------------------

static void UpdateTime(ID3D11Buffer *buffer)
{
  static auto &time = GetGame()->Time;
  static auto LastUpdated = time.Frame - 1;

  if (LastUpdated >= time.Frame)
    return;

  LastUpdated = time.Frame;

  D3D11_MAPPED_SUBRESOURCE map;
  HRESULT hr;

  auto &context = GetGame()->GameDevice->DeviceContext;
  hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
  CHECK_HRESULT(hr);

  auto runtime =
    time.RunningTime.count() *
    GameTime::clock::period::num /
    static_cast<long double>(GameTime::clock::period::den);

  float &resTime = *static_cast<float *>(map.pData);
  resTime = static_cast<float>(runtime);

  context->Unmap(buffer, 0);
}

// ----------------------------------------------------------------------------

void Shader::Draw(unsigned indexCount)
{
  static auto *timeRes = CreateTimeResource();
  UpdateTime(timeRes);

  using namespace DirectX;
  auto *context = device->DeviceContext;
  HRESULT result;

  D3D11_MAPPED_SUBRESOURCE mappedCameraRes;
  result = context->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD,
                        0, &mappedCameraRes);
  CHECK_HRESULT(result);

  Camera *mCamera = static_cast<Camera *>(mappedCameraRes.pData);

  mCamera->worldMatrix = XMMatrixTranspose(camera->worldMatrix.get());
  mCamera->viewMatrix = XMMatrixTranspose(camera->viewMatrix.get());
  mCamera->projectionMatrix = XMMatrixTranspose(camera->projectionMatrix.get());

  context->Unmap(cameraBuffer, 0);

  ID3D11Buffer *buffers[] =
    {
      cameraBuffer,
      timeRes
    };

  context->VSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
  context->IASetInputLayout(vertexLayout);
  context->VSSetShader(vertexShader, nullptr, 0);
  context->PSSetShader(pixelShader, nullptr, 0);

  context->DrawIndexed(indexCount, 0, 0);
}

// ----------------------------------------------------------------------------

void Shader::InitCameraBuffer()
{
  D3D11_BUFFER_DESC cameraBufferDesc;

  cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  cameraBufferDesc.ByteWidth = sizeof(Camera);
  cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cameraBufferDesc.MiscFlags = 0;
  cameraBufferDesc.StructureByteStride = 0;

  HRESULT result = device->Device->CreateBuffer(&cameraBufferDesc,
                                                nullptr, &cameraBuffer);
  CHECK_HRESULT(result);
}

// ----------------------------------------------------------------------------

std::unordered_map<std::string, Shader *> RegisteredShaders;

// ----------------------------------------------------------------------------