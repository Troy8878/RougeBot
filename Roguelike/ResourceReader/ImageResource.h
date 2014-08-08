/*********************************
 * ImageResource.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "Helpers\SharedArray.h"
#include "Helpers\UseDirectX.h"

struct ImageResource
{
  enum class Format
  {
    RGB,
    RGBA,
    BGR,
    BGRA,
  };

  ImageResource() = default;
  ImageResource(int width, int height, Format format, shared_array<byte> data)
    : width(width), height(height), format(format), data(data)
  {
  }

  UINT width, height;
  Format format;
  shared_array<byte> data;

  void to32BitColor(byte *destination, size_t dest_size);
  template <size_t dest_size>
  void to32BitColor(byte (&destination)[dest_size])
  {
    to32BitColor(destination, dest_size);
  }
  void to32BitColor(shared_array<byte>& destination)
  {
    to32BitColor(destination, destination.size());
  }

  static ImageResource fromFile(const std::wstring& file);
};


