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

  void Initialize(Entity *owner, const std::string& name) override;

  PROPERTY(get = _GetTextures) const std::vector<Texture2D>& Textures;
  PROPERTY(get = _GetTextureCount) size_t TextureCount;

  void AddTexture(json::value Textureef);
  void RemoveTexture(size_t index);

  mrb_value GetRubyWrapper() override;

  static TextureComponentFactory factory;

private:
  std::vector<Texture2D> textures;

  static Texture2D ConstructTexture(json::value definition);
  static Texture2D ConstructZipped(json::value definition);

public:
  const std::vector<Texture2D>& _GetTextures() const { return textures; }
  size_t _GetTextureCount() const { return textures.size(); }
};

// ----------------------------------------------------------------------------

class TextureComponentFactory : public IComponentFactory
{
public:
  TextureComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
