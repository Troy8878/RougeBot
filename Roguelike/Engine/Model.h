/*********************************
 * Model.h
 * Connor Hilarides
 * Created 2014/07/06
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Camera.h"
#include "Texture.h"

// ----------------------------------------------------------------------------

struct Shader;

// ----------------------------------------------------------------------------

struct BasicVertex final
{
  math::Vector position;
  math::Vector color;
};

// ----------------------------------------------------------------------------

struct TexturedVertex final
{
  inline TexturedVertex()
    : position(0, 0, 0, 0), color(1, 1, 1, 1), texture(0, 0)
  {
  }

  inline TexturedVertex(const math::Vector &pos,
                        const math::Vector &color = {},
                        const math::Vector2D &tex = {})
    : position(pos), color(color), texture(tex)
  {
  }

  math::Vector position;
  math::Vector color;
  math::Vector2D texture;
};

// ----------------------------------------------------------------------------

class Model
{
public:
  Model(ID3D11Buffer *vertexBuffer, UINT vertexCount,
        ID3D11Buffer *indexBuffer, UINT indexCount, UINT stride);

  Model(ID3D11Device *device,
        void *vertices, UINT vertexCount,
        UINT *indices, UINT indexCount,
        UINT stride);

  template <typename VertexType>
  Model(ID3D11Device *device,
        VertexType *vertices, UINT vertexCount,
        UINT *indices, UINT indexCount);

  template <typename VertexType, UINT vertexCount, UINT indexCount>
  Model(ID3D11Device *device,
        VertexType (&vertices)[vertexCount],
        UINT (&indices)[indexCount]);

  void XM_CALLCONV Draw(DirectX::FXMMATRIX worldTransform) const;

  Shader *shader;
  Texture2D texture;
  Texture2D tintTexture;
  math::Vector tint = {1, 1, 1, 1};

  ID3D11Buffer *GetVB()
  {
    return _vertexBuffer;
  }

  ID3D11Buffer *GetIB()
  {
    return _indexBuffer;
  }

  UINT GetVC()
  {
    return _vertexCount;
  }

  UINT GetIC()
  {
    return _indexCount;
  }

  UINT GetStride()
  {
    return _stride;
  }

private:
  ID3D11Buffer *_vertexBuffer, *_indexBuffer;
  UINT _vertexCount, _indexCount, _stride;
};

// ----------------------------------------------------------------------------

template <typename VertexType>
inline
Model::Model(ID3D11Device *device,
             VertexType *vertices, UINT vertexCount,
             UINT *indices, UINT indexCount)
  : Model{device, vertices, vertexCount, indices, indexCount, ARRAY_STRIDE(vertices)}
{
}

// ----------------------------------------------------------------------------

template <typename VertexType, UINT vertexCount, UINT indexCount>
inline
Model::Model(ID3D11Device *device,
             VertexType (&vertices)[vertexCount],
             UINT (&indices)[indexCount])
  : Model{device, vertices, vertexCount, indices, indexCount}
{
}

// ----------------------------------------------------------------------------

bool operator==(const BasicVertex &v1, const BasicVertex &v2);
bool operator==(const TexturedVertex &v1, const TexturedVertex &v2);

// ----------------------------------------------------------------------------