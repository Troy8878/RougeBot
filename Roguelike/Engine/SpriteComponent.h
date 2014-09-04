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

#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class SpriteComponentFactory;
class TransformComponent;

// ----------------------------------------------------------------------------

class SpriteComponent : public Component, public Drawable
{
public:
  // Constructors and Destructor
  SpriteComponent(Texture2D texture, 
                  Shader *shader, RenderSet *set);
  SpriteComponent(const std::vector<Texture2D>& textures, 
                  Shader *shader, RenderSet *set);
  ~SpriteComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void Draw() override;

  IR_PROPERTY(Model *, UnitSquare);
  IR_PROPERTY(Shader *, ModelShader);
  IR_PROPERTY(TransformComponent *, Transform);
  IR_PROPERTY(size_t, TextureIndex);
  PROPERTY(get = _GetTextureCount) size_t TextureCount;
  
  ruby::ruby_value GetRubyWrapper() override;

  // Component factory to make sprite component
  static SpriteComponentFactory factory;

private:
  std::vector<Texture2D> _textures;
  RenderSet *renderTarget = nullptr;

  static Model *GetSpriteModel();

public:
  size_t _GetTextureCount();
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
