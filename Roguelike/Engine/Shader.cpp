/*********************************
 * Shader.cpp
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#include "Common.h"
#include "Shader.h"
#include "Camera.h"

Shader::~Shader()
{
  releaseDXInterface(vertexShader);
  releaseDXInterface(pixelShader);
  releaseDXInterface(cameraBuffer);
}

Shader *Shader::loadShader(
  GraphicsDevice *device,
  const fs::wpath& vertexfile,
  const fs::wpath& pixelfile)
{
  HRESULT result;
  auto *shader = new Shader;
  shader->device = device;

  shader->vertexShaderData = fs::file_reader::readAllBytes(vertexfile);
  shader->pixelShaderData = fs::file_reader::readAllBytes(pixelfile);

  assert(shader->vertexShaderData.data);
  assert(shader->pixelShaderData.data);

  result = device->device()->CreateVertexShader(
    shader->vertexShaderData.data,
    shader->vertexShaderData.size,
    nullptr, &shader->vertexShader);
  CHECK_HRESULT(result);

  setDXDebugName(shader->vertexShader, L"VertexShader{" + vertexfile.filename() + L"}");

  result = device->device()->CreatePixelShader(
    shader->pixelShaderData.data,
    shader->pixelShaderData.size,
    nullptr, &shader->pixelShader);
  CHECK_HRESULT(result);

  setDXDebugName(shader->pixelShader, L"PixelShader{" + pixelfile.filename() + L"}");

  return shader;
}

void Shader::draw(unsigned indexCount)
{
  using namespace DirectX;
  auto *context = device->deviceContext();
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

void Shader::initCameraBuffer()
{
  D3D11_BUFFER_DESC cameraBufferDesc;

  cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  cameraBufferDesc.ByteWidth = sizeof(Camera);
  cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cameraBufferDesc.MiscFlags = 0;
  cameraBufferDesc.StructureByteStride = 0;

  HRESULT result = device->device()->CreateBuffer(&cameraBufferDesc,
                                                  nullptr, &cameraBuffer);
  CHECK_HRESULT(result);
}

void Shader::initializeBasicShader()
{
  initCameraBuffer();

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

  auto result = device->device()->
    CreateInputLayout(polygonLayout, ARRAYSIZE(polygonLayout),
                      vertexShaderData.data, vertexShaderData.size,
                      &vertexLayout);
  CHECK_HRESULT(result);
}

