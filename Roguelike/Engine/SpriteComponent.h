/*********************************
 * SpriteComponent.h
 * Jake Robsahm
 * Created 2014/08/19
 *********************************/

#pragma once

#include "Common.h"
#include "Texture.h"
#include "Model.h"
#include "RenderSet.h"
#include "TextureComponent.h"

#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class SpriteComponentFactory;
class TransformComponent;

// ----------------------------------------------------------------------------

class SpriteComponent : public Component, public Drawable
{
public:
  // Constructors and Destructor
  SpriteComponent(Shader *shader, RenderSet *set);
  ~SpriteComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void Draw() override;
  void OnSetDestroyed() override { renderTarget = nullptr; }

  IR_PROPERTY(Model *, UnitSquare);
  IR_PROPERTY(Shader *, ModelShader);
  PROPERTY(get = _GetTextureCount) size_t TextureCount;
  size_t TextureIndex;

  Texture2D GetTexture(size_t index) { return _texture ? _texture->Textures[index] : Texture2D(); }

  mrb_value GetRubyWrapper() override;

  // Component factory to make sprite component
  static SpriteComponentFactory factory;

private:
  RenderSet *renderTarget = nullptr;
  TextureComponent *_texture = nullptr;

  static Model *GetSpriteModel();

public:
  size_t _GetTextureCount() { return _texture ? _texture->TextureCount : 0; }
};

// ----------------------------------------------------------------------------

class SpriteComponentFactory : public IComponentFactory
{
public:
  SpriteComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
