/*********************************
 * VectorHelp.h
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include <iostream>


namespace math
{
  using namespace DirectX;

  const float pi = 3.14159265358979f;

  class Vector : public XMFLOAT4A
  {
  public:
    inline Vector() = default;
    inline Vector(CXMVECTOR v1)
    {
      XMStoreFloat4A(this, v1);
    }

    inline Vector(float x, float y, float z, float w)
      : Vector(XMVectorSet(x, y, z, w))
    {
    }

    inline XMVECTOR XM_CALLCONV get() const { return XMLoadFloat4A(this); }
    inline Vector& XM_CALLCONV operator=(FXMVECTOR v1)
    {
      XMStoreFloat4A(this, v1);
      return *this;
    }

    inline float *buffer() { return &x; }

    inline bool operator==(const Vector& other)
    {
      return 
        x == other.x &&
        y == other.y &&
        z == other.z &&
        w == other.w;
    }
  };

  class Vector2D : public XMFLOAT2A
  {
  public:
    inline Vector2D() = default;
    inline Vector2D(CXMVECTOR v1)
    {
      XMStoreFloat2A(this, v1);
    }

    inline Vector2D(float x, float y)
      : Vector2D(XMVectorSet(x, y, 0, 0))
    {
    }

    inline XMVECTOR XM_CALLCONV get() const { return XMLoadFloat2A(this); }
    inline Vector2D& XM_CALLCONV operator=(FXMVECTOR v1)
    {
      XMStoreFloat2A(this, v1);
      return *this;
    }

    inline bool operator==(const Vector2D& other)
    {
      return
        x == other.x &&
        y == other.y;
    }

    inline bool operator!=(const Vector2D& other)
    {
      return !(*this == other);
    }
  };

  class Matrix
  {
    XMFLOAT4X4A _value;

  public:
    inline Matrix() = default;
    inline Matrix(CXMMATRIX m1)
    {
      XMStoreFloat4x4A(&_value, m1);
    }

    inline XMMATRIX XM_CALLCONV get() { return XMLoadFloat4x4A(&_value); }
    inline Matrix& XM_CALLCONV operator=(FXMMATRIX m1)
    {
      XMStoreFloat4x4A(&_value, m1);
      return *this;
    }
  };

  inline XMVECTOR XM_CALLCONV createVector(float x, float y, float z)
  {
    return XMVectorSet(x, y, z, 0);
  }

  inline XMVECTOR XM_CALLCONV createPoint(float x, float y, float z)
  {
    return XMVectorSet(x, y, z, 1);
  }

  inline XMMATRIX XM_CALLCONV joinMatrices(std::initializer_list<XMMATRIX> matrices)
  {
    XMMATRIX result = XMMatrixIdentity();
    
    for (auto matrix : matrices)
    {
      result = XMMatrixMultiply(result, matrix);
    }

    return result;
  }
  
  template <size_t count>
  inline void XM_CALLCONV transformMany(FXMMATRIX matrix, Vector (&input)[count], Vector (&output)[count])
  {
    transformMany(matrix, input, output, count);
  }

  inline void XM_CALLCONV transformMany(FXMMATRIX matrix, Vector *input, Vector *output, size_t count)
  {
    const ptrdiff_t instride = reinterpret_cast<byte *>(input + 1) - 
                               reinterpret_cast<byte *>(input);
    const ptrdiff_t outstride = reinterpret_cast<byte *>(output + 1) - 
                                reinterpret_cast<byte *>(output);

    // It's okay to reinterpret this because we use a stride,
    // and an XMFLOAT4 is the first element of the structure
    XMVector4TransformStream(reinterpret_cast<XMFLOAT4 *>(output), outstride, 
                             reinterpret_cast<XMFLOAT4 *>(input), instride, count, matrix);
  }
}

inline DirectX::XMVECTOR XM_CALLCONV operator*(DirectX::FXMMATRIX matrix, DirectX::CXMVECTOR vect)
{
  return XMVector4Transform(vect, matrix);
}

inline std::ostream& XM_CALLCONV operator<<(std::ostream& os, DirectX::CXMVECTOR v1)
{
  using namespace DirectX;

  os << "<";
  os << XMVectorGetX(v1);
  os << ", ";
  os << XMVectorGetY(v1);
  os << ", ";
  os << XMVectorGetZ(v1);
  os << ", ";
  os << XMVectorGetW(v1);
  os << ">";

  return os;
}



