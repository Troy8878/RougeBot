/*********************************
 * TextureComponent.h
 * Connor Hilarides
 * Created 2014/09/14
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class TextureComponentFactory;

// ----------------------------------------------------------------------------

class TextureComponent : public Component
{
public:
  TextureComponent();

  void Initialize(Entity *owner, const std::string &name) override;

  PROPERTY(get = _GetTextures) const std::vector<Texture2D> &Textures;
  PROPERTY(get = _GetTextureCount) size_t TextureCount;

  void AddTexture(json::value definition);
  void RemoveTexture(size_t index);

  mrb_value GetRubyWrapper() override;

  static TextureComponentFactory factory;

private:
  std::vector<Texture2D> textures;

public:
  const std::vector<Texture2D> &_GetTextures() const
  {
    return textures;
  }

  size_t _GetTextureCount() const
  {
    return textures.size();
  }

  // For the sake of ruby
  size_t _GetTextureCount()
  {
    return textures.size();
  }
};

// ----------------------------------------------------------------------------

class TextureComponentFactory final : public IComponentFactory
{
public:
  TextureComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------