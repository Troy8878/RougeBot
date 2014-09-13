/*********************************
 * TextureZip.h
 * Connor Hilarides
 * Created 2014/09/12
 *********************************/

#pragma once

#include "Common.h"
#include "Texture.h"

// ----------------------------------------------------------------------------

class TextureZip
{
public:
  TextureZip(const std::vector<std::string>& assets);

  struct TextureMapping
  {
    math::Vector2D topLeft;
    math::Vector2D topRight;
    math::Vector2D bottomLeft;
    math::Vector2D bottomRight;
  };

  typedef std::unordered_map<std::string, TextureMapping> mapping_t;

  PROPERTY(get = _GetTexture) Texture2D Texture;
  PROPERTY(get = _GetMappings) const mapping_t& Mappings;

private:
  Texture2D _texture;
  mapping_t _mappings;

  TextureMapping PrintImageToBuffer(ImageResource& buffer, const ImageResource& image, 
                                    UINT x, UINT y);

public:
  Texture2D _GetTexture() { return _texture; }
  const mapping_t& _GetMappings() { return _mappings; }
};

// ----------------------------------------------------------------------------

