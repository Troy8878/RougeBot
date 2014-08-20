/*********************************
 * SpriteComponent.h
 * Jake Robsahm
 * Created 2014/08/19
 *********************************/

#pragma once

#include "Common.h"
#include "Texture.h"
#include "Model.h"

#include "Helpers\BucketAllocator.h"

class SpriteComponentFactory;

class SpriteComponent : public Component
{
public:
  // Constructors and Destructor
  SpriteComponent(Texture2D texture);
  SpriteComponent(const std::vector<Texture2D>& textures);
  ~SpriteComponent();

  void Initialize(Entity *owner, const std::string& name) override;

  IRW_PROPERTY(Camera *, RenderCamera);
  IR_PROPERTY(Model *, UnitSquare);
  
  // Component factory to make sprite component
  static SpriteComponentFactory factory;

private:
  std::vector<Texture2D> _textures;

  void Draw(Events::EventMessage&);

  static Model *GetSpriteModel();
};

class SpriteComponentFactory : public ComponentFactory
{
public:
  SpriteComponentFactory();

  Component *operator()(void *memory, component_factory_data& data) override;

  ::Allocator *GetAllocator() { return &allocator; }

private:
  BucketAllocator allocator;
};
