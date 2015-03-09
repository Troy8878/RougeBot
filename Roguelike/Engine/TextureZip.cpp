/*********************************
 * TextureZip.cpp
 * Connor Hilarides
 * Created 2014/09/12
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "TextureZip.h"
#include "ResourceReader\ImageResource.h"

// ----------------------------------------------------------------------------

TextureZip::TextureZip(const std::vector<std::string> &assets)
{
  assert(assets.size());

  std::vector<ImageResource> images;
  images.reserve(assets.size());
  for (auto &asset : assets)
  {
    images.push_back(ImageResource::fromAsset("Textures", asset));
  }

  ImageResource buffer;
  buffer.format = ImageResource::Format::BGRA;
  buffer.width = 0;
  buffer.height = 0;

  for (auto &image : images)
  {
    if (buffer.width < image.width)
      buffer.width = image.width;

    buffer.height += image.height;
  }

  buffer.data = shared_array<byte>(buffer.width * buffer.height * sizeof(UINT));

  UINT y = 0;
  for (size_t i = 0; i < images.size(); ++i)
  {
    auto &image = images[i];

    _mappings[assets[i]] = PrintImageToBuffer(buffer, image, 0, y);
    y += image.height;
  }

  _texture = Texture2D{GetGame()->GameDevice->Device, buffer};

  std::ostringstream buf;
  buf << "Zip[";

  bool first = true;
  for (auto &asset : assets)
  {
    if (first)
      first = false;
    else
      buf << ", ";

    buf << asset;
  }
  buf << "]";

  _texture._res->name = buf.str();
}

// ----------------------------------------------------------------------------

struct PrintingContext
{
  ImageResource *buffer;
  const ImageResource *image;
  UINT bx, ix;
  UINT by, iy;

  void WriteRun()
  {
    uint32_t *bbuf = reinterpret_cast<uint32_t *>(buffer->data.get());
    const uint32_t *ibuf = reinterpret_cast<const uint32_t *>(image->data.get());

    bbuf += buffer->width * by;
    ibuf += image->width * iy;

    for (UINT i = 0; i < image->width; ++i)
    {
      bbuf[bx++] = ibuf[ix++];
    }
  }
};

// ----------------------------------------------------------------------------

auto TextureZip::PrintImageToBuffer(ImageResource &buffer, const ImageResource &image,
                                    UINT x, UINT y) -> TextureMapping


{
  assert(buffer.format == ImageResource::Format::BGRA);
  assert(image.format == ImageResource::Format::BGRA);

  TextureMapping mapping;
  mapping.topLeft = math::Vector2D
    {
      float(x) / float(buffer.width),
      float(y) / float(buffer.height)
    };
  mapping.topRight = math::Vector2D
    {
      float(x + image.width) / float(buffer.width),
      float(y) / float(buffer.height)
    };
  mapping.bottomLeft = math::Vector2D
    {
      float(x) / float(buffer.width),
      float(y + image.height) / float(buffer.height)
    };
  mapping.bottomRight = math::Vector2D
    {
      float(x + image.width) / float(buffer.width),
      float(y + image.height) / float(buffer.height)
    };

  PrintingContext context;
  context.buffer = &buffer;
  context.image = &image;

  for (UINT i = 0; i < image.height; ++i)
  {
    context.bx = 0;
    context.ix = 0;
    context.by = y + i;
    context.iy = i;

    context.WriteRun();
  }

  return mapping;
}

// ----------------------------------------------------------------------------