/*********************************
 * Shader.cpp
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Game.h"

// ----------------------------------------------------------------------------

Shader::~Shader()
{
  ReleaseDXInterface(vertexShader);
  ReleaseDXInterface(pixelShader);
  ReleaseDXInterface(cameraBuffer);
}

// ----------------------------------------------------------------------------

Shader *Shader::LoadShader(
  GraphicsDevice *device,
  const std::string& vertexAsset,
  const std::string& pixelAsset)
{
  HRESULT result;
  auto *shader = new Shader;
  shader->device = device;

  auto& respack = GetGame()->Respack;
  auto *shadersContainer = respack["Shaders"];
  RELEASE_AFTER_SCOPE(shadersContainer);

  auto *vertexRes = shadersContainer->getResource(vertexAsset);
  RELEASE_AFTER_SCOPE(vertexRes);

  auto pixelRes = shadersContainer->getResource(pixelAsset);
  RELEASE_AFTER_SCOPE(pixelRes);

  shader->vertexShaderData = shared_array<byte>{vertexRes->getData(), vertexRes->getSize()};
  shader->pixelShaderData = shared_array<byte>{pixelRes->getData(), pixelRes->getSize()};

  assert(shader->vertexShaderData);
  assert(shader->pixelShaderData);

  result = device->Device->CreateVertexShader(
    shader->vertexShaderData,
    shader->vertexShaderData.size(),
    nullptr, &shader->vertexShader);
  CHECK_HRESULT(result);

  setDXDebugName(shader->vertexShader, L"VertexShader{" + widen(vertexAsset) + L"}");

  result = device->Device->CreatePixelShader(
    shader->pixelShaderData,
    shader->pixelShaderData.size(),
    nullptr, &shader->pixelShader);
  CHECK_HRESULT(result);

  setDXDebugName(shader->pixelShader, L"PixelShader{" + widen(pixelAsset) + L"}");

  return shader;
}

// ----------------------------------------------------------------------------

void Shader::Draw(unsigned indexCount)
{
  using namespace DirectX;
  auto *context = device->DeviceContext;
  HRESULT result;

  D3D11_MAPPED_SUBRESOURCE mappedCameraRes;
  result = context->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCameraRes);
  CHECK_HRESULT(result);

  Camera *mappedCamera = static_cast<Camera *>(mappedCameraRes.pData);
  mappedCamera->worldMatrix = XMMatrixTranspose(camera->worldMatrix.get());
  mappedCamera->viewMatrix = XMMatrixTranspose(camera->viewMatrix.get());
  mappedCamera->projectionMatrix = XMMatrixTranspose(camera->projectionMatrix.get());
  context->Unmap(cameraBuffer, 0);
  context->VSSetConstantBuffers(0, 1, &cameraBuffer);

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

void Shader::InitializeBasicShader()
{
  InitCameraBuffer();

  #pragma region Polygon Layout
  static const D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
  {
    {
      "POSITION",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      0,
      0,
      D3D11_INPUT_PER_VERTEX_DATA,
      0
    },
    {
      "COLOR",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      0,
      D3D11_APPEND_ALIGNED_ELEMENT,
      D3D11_INPUT_PER_VERTEX_DATA,
      0
    }
  };
  #pragma endregion

  auto result = device->Device->
    CreateInputLayout(polygonLayout, ARRAYSIZE(polygonLayout),
                      vertexShaderData, vertexShaderData.size(),
                      &vertexLayout);
  CHECK_HRESULT(result);
}

// ----------------------------------------------------------------------------

void Shader::InitializeTexturedShader()
{
  InitCameraBuffer();

  #pragma region Polygon Layout
  static const D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
  {
    {
      "POSITION",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      0,
      0,
      D3D11_INPUT_PER_VERTEX_DATA,
      0
    },
    {
      "COLOR",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      0,
      D3D11_APPEND_ALIGNED_ELEMENT,
      D3D11_INPUT_PER_VERTEX_DATA,
      0
    },
    {
      "TEXCOORD",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      0,
      D3D11_APPEND_ALIGNED_ELEMENT,
      D3D11_INPUT_PER_VERTEX_DATA,
      0
    }
  };
  #pragma endregion

  auto result = device->Device->
    CreateInputLayout(polygonLayout, ARRAYSIZE(polygonLayout),
                      vertexShaderData, vertexShaderData.size(),
                      &vertexLayout);
  CHECK_HRESULT(result);
}

// ----------------------------------------------------------------------------

std::unordered_map<std::string, Shader *> RegisteredShaders;

// ----------------------------------------------------------------------------

