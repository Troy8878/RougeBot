/*********************************
 * ImageResource.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

struct ImageResource
{
  typedef unsigned __int8 byte;

  enum class Format
  {
    R8G8B8,
    R8G8B8A8,
  };

  int width, height;
  Format format;
  byte *data;

  void Dispose();
};


