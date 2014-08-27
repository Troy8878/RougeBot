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
  SpriteComponent(Texture2D texture, Shader *shader);
  SpriteComponent(const std::vector<Texture2D>& textures, Shader *shader);
  ~SpriteComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void Draw() override;

  IR_PROPERTY(Model *, UnitSquare);
  IR_PROPERTY(Shader *, ModelShader);
  IR_PROPERTY(TransformComponent *, Transform);
  
  // Component factory to make sprite component
  static SpriteComponentFactory factory;

private:
  std::vector<Texture2D> _textures;

  static Model *GetSpriteModel();
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
