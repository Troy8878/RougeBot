/*********************************
 * StandardShapes.cpp
 * Connor Hilarides
 * Created 2014/07/03
 *********************************/

#include "Common.h"
#include "StandardShapes.h"
#include "Colors.h"

static std::random_device shape_rng;

namespace Shapes
{
  using namespace DirectX;
  
  Model *makeCircle(ID3D11Device *device, unsigned sides, float radius, math::Vector color)
  {
    assert(sides >= 3);
    assert(radius > 0);

    XMMATRIX rotate = XMMatrixRotationZ(math::pi / sides * 2);
    XMVECTOR vertex = XMVectorSet(radius, 0, 0, 1);
    XMVECTOR texvertex = XMVectorSet(-0.5f, 0, 0, 1);
    XMVECTOR texoffset = XMVectorSet(0.5f, 0.5f, 0, 0);

    UINT vertexCount = sides + 1;
    UINT indexCount = sides * 3;
    
    TexturedVertex *vertices = new TexturedVertex[vertexCount];
    vertices[sides].position = XMVectorSet(0, 0, 0, 1);
    vertices[sides].color = color;
    vertices[sides].texture = XMVectorSet(0.5f, 0.5f, 0, 1);

    unsigned *indices = new unsigned[indexCount];
    
    for (unsigned i = 0; i < sides; ++i)
    {
      vertices[i].position = vertex;
      vertices[i].color = color;
      vertices[i].texture = texvertex + texoffset;

      indices[i * 3 + 0] = sides;
      indices[i * 3 + 1] = i;
      indices[i * 3 + 2] = (i + 1) % sides;

      vertex = rotate * vertex;
      texvertex = rotate * texvertex;
    }
    
    auto *model = new Model{device, vertices, vertexCount, indices, indexCount};

    delete[] vertices;
    delete[] indices;

    return model;
  }

  Model *makeCube(ID3D11Device *device, float size, math::Vector color)
  {
    const float csize = size / 2.0f;

    BasicVertex vertices[] =
    {
      {{ csize,  csize,  csize, 1}, color},
      {{-csize,  csize,  csize, 1}, color},
      {{ csize, -csize,  csize, 1}, color},
      {{-csize, -csize,  csize, 1}, color},
      {{ csize,  csize, -csize, 1}, color},
      {{-csize,  csize, -csize, 1}, color},
      {{ csize, -csize, -csize, 1}, color},
      {{-csize, -csize, -csize, 1}, color},
    };

    UINT indices[] =
    {
      0, 1, 2, 2, 1, 3,
      0, 2, 4, 2, 6, 4,
      5, 4, 6, 5, 6, 7,
      1, 5, 7, 1, 7, 3,
      0, 5, 1, 0, 4, 5,
      2, 3, 6, 6, 3, 7
    };

    const UINT vertexCount = ARRAYSIZE(vertices);
    const UINT indexCount = ARRAYSIZE(indices);

    if (color == Colors::Transparent)
    {
      for (auto& vertex : vertices)
      {
        std::uniform_real_distribution<float> color_rand;

        vertex.color = math::Vector
        {
          color_rand(shape_rng),
          color_rand(shape_rng),
          color_rand(shape_rng),
          1
        };
      }
    }

    return new Model{device, vertices, vertexCount, indices, indexCount};
  }
}
