/*********************************
 * Model.h
 * Connor Hilarides
 * Created 2014/07/06
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Camera.h"

struct Shader;

struct BasicVertex final
{
  math::Vector position;
  math::Vector color;
};

struct TexturedVertex final
{
  math::Vector position;
  math::Vector color;
  math::Vector2D texture;
};

class Model
{
public:
  Model(ID3D11Buffer *vertexBuffer, UINT vertexCount, 
        ID3D11Buffer *indexBuffer, UINT indexCount);

  template <typename VertexType>
  Model(ID3D11Device *device,
        VertexType *vertices, UINT vertexCount,
        UINT *indexBuffer, UINT indexCount);

  void XM_CALLCONV draw(DirectX::FXMMATRIX worldTransform) const;

  void setShader(Shader *shader) { _shader = shader; }
  Shader *getShader() const { return _shader; }

private:
  ID3D11Buffer *_vertexBuffer, *_indexBuffer;
  UINT _vertexCount, _indexCount;

  Shader *_shader;
};


template <typename VertexType>
Model::Model(ID3D11Device *device,
             VertexType *vertices, UINT vertexCount,
             UINT *indices, UINT indexCount)
             : _vertexCount(vertexCount), _indexCount(indexCount)
{
  D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;
  HRESULT result;

  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.StructureByteStride = ARRAY_STRIDE(vertices);
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

