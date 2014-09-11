/*********************************
 * VectorHelp.h
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include <iostream>
#include <algorithm>

namespace math
{
  using namespace DirectX;

  const float pi = 3.14159265358979f;

  __declspec(align(16)) class Vector : public XMFLOAT4A
  {
  public:
    inline Vector() = default;
    inline Vector(CXMVECTOR v1)
    {
      XMStoreFloat4A(this, v1);
    }

    inline Vector(float x, float y, float z, float w)
      : XMFLOAT4A(x, y, z, w)
    {
    }

    inline XMVECTOR XM_CALLCONV get() const { return XMLoadFloat4A(this); }
    inline Vector& XM_CALLCONV operator=(FXMVECTOR v1)
    {
      XMStoreFloat4A(this, v1);
      return *this;
    }

    inline float *buffer() { return &x; }

    inline bool operator==(const Vector& other) const
    {
      return 
        x == other.x &&
        y == other.y &&
        z == other.z &&
        w == other.w;
    }

    static math::Vector VectorFromJson(json::value value)
    {
      auto nums = value.as_array_of<json::value::number_t>();
      while (nums.size() < 4)
        nums.push_back(0);

      return math::Vector{(float)nums[0],(float)nums[1],(float)nums[2],(float)nums[3]};
    }

    inline operator XMVECTOR() const { return get(); }
  };

  __declspec(align(16)) class Vector2D : public XMFLOAT2A
  {
  public:
    inline Vector2D() = default;
    inline Vector2D(CXMVECTOR v1)
    {
      XMStoreFloat2A(this, v1);
    }

    inline Vector2D(float x, float y)
      : XMFLOAT2A(x, y)
    {
    }

    inline XMVECTOR XM_CALLCONV get() const { return XMLoadFloat2A(this); }
    inline Vector2D& XM_CALLCONV operator=(FXMVECTOR v1)
    {
      XMStoreFloat2A(this, v1);
      return *this;
    }

    inline bool operator==(const Vector2D& other) const
    {
      return
        x == other.x &&
        y == other.y;
    }

    inline bool operator!=(const Vector2D& other) const
    {
      return !(*this == other);
    }

    inline operator XMVECTOR() const { return get(); }
  };

  __declspec(align(16)) class Matrix
  {
    XMFLOAT4X4A _value;

  public:
    inline Matrix() = default;
    inline Matrix(CXMMATRIX m1)
    {
      XMStoreFloat4x4A(&_value, m1);
    }

    inline XMMATRIX XM_CALLCONV get() const { return XMLoadFloat4x4A(&_value); }
    inline Matrix& XM_CALLCONV operator=(FXMMATRIX m1)
    {
      XMStoreFloat4x4A(&_value, m1);
      return *this;
    }

    inline operator XMMATRIX() const { return get(); }
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

  /**
    Converts degrees to radians
  */
  inline float degrees(float degs)
  {
    return degs * pi / 180.f;
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

inline std::ostream& XM_CALLCONV operator<<(std::ostream& os, DirectX::CXMMATRIX m1)
{
  size_t longest_num = 1;
  for (unsigned i = 0; i < 16; ++i)
  {
    longest_num = std::max(longest_num, std::to_string(m1.r[i/4].m128_f32[i%4]).size());
  }

  os << '\xDA'; // ┌
  for (unsigned i = 0; i < (longest_num * 4) + 3; ++i)
    os << ' ';
  os << '\xBF' << std::endl; // ┐

  for (unsigned i = 0; i < 4; ++i)
  {
    os << '\xB3'; // │
    for (unsigned j = 0; j < 4; ++j)
    {
      auto numstr = std::to_string(m1.r[i].m128_f32[j]);
      for (unsigned i = 0; i < longest_num - numstr.size(); ++i)
        os << ' ';
      os << numstr;
      if (j < 3)
        os << ' ';
    }
    os << '\xB3' << std::endl; // │
  }

  os << '\xC0'; // └
  for (unsigned i = 0; i < (longest_num * 4) + 3; ++i)
    os << ' ';
  os << '\xD9' << std::endl; // ┘

  return os;
}


