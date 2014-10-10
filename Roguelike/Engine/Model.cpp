/*********************************
 * Model.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#include "Common.h"
#include "Model.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

static BasicVertex *BasicVertexPtr;

// ----------------------------------------------------------------------------

Model::Model(ID3D11Buffer *vertexBuffer, UINT vertexCount, 
             ID3D11Buffer *indexBuffer, UINT indexCount, UINT stride)
             : _vertexBuffer(vertexBuffer), _vertexCount(vertexCount),
               _indexBuffer(indexBuffer), _indexCount(indexCount), _stride(stride)
{
}

// ----------------------------------------------------------------------------

Model::Model(ID3D11Device *device,
             void *vertices, UINT vertexCount,
             UINT *indices, UINT indexCount,
             UINT stride)
             : _vertexCount(vertexCount), _indexCount(indexCount), _stride(stride)
{
  D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;
  HRESULT result;

  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.StructureByteStride = stride;
  vertexBufferDesc.ByteWidth = vertexBufferDesc.StructureByteStride * vertexCount;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;

  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
  CHECK_HRESULT(result);

  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.StructureByteStride = ARRAY_STRIDE(indices);
  indexBufferDesc.ByteWidth = indexBufferDesc.StructureByteStride * indexCount;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;

  indexData.pSysMem = indices;
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;

  result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
  CHECK_HRESULT(result);
}

// ----------------------------------------------------------------------------

void XM_CALLCONV Model::Draw(DirectX::FXMMATRIX worldTransform) const
{
  shader->camera->worldMatrix = worldTransform;

  unsigned offset = 0;

  auto context = shader->device->DeviceContext;
  context->IASetVertexBuffers(0, 1, &_vertexBuffer, &_stride, &offset);
  context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, offset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  static auto nulltex = Texture2D::GetNullTexture();

  if (texture && !shader->device->WireframeDraw)
    context->PSSetShaderResources(0, 1, &texture.ShaderRes);
  else
    context->PSSetShaderResources(0, 1, &nulltex.ShaderRes);

  shader->Draw(_indexCount);
}

// ----------------------------------------------------------------------------

bool operator==(const BasicVertex& v1, const BasicVertex& v2);

// ----------------------------------------------------------------------------

bool operator==(const TexturedVertex& v1, const TexturedVertex& v2)
{
  using namespace DirectX;

  static const XMVECTOR epsilon = XMVectorSet(0.0001f, 0.0001f, 0.0001f, 0.0001f);

  return XMVector3NearEqual(v1.position, v2.position, epsilon) &&
         XMVector3NearEqual(v1.color, v2.color, epsilon) &&
         XMVector2NearEqual(v1.texture, v2.texture, epsilon);
}

// ----------------------------------------------------------------------------

