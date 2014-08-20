/*********************************
 * SpriteComponent.h
 * Jake Robsahm
 * Created 2014/08/19
 *********************************/

#pragma once

#include "Common.h"
#include "Texture.h"
#include "Model.h"

class SpriteComponentFactory;

class SpriteComponent : public Component
{
public:
  // Constructors and Destructor
  SpriteComponent(Texture2D texture);
  SpriteComponent(const std::vector<Texture2D>& textures);
  ~SpriteComponent();
  
  // Component factory to make sprite component
  static SpriteComponentFactory factory;

private:
  std::vector<Texture2D> _textures;

  static Model *GetSpriteModel();
  
};

class SpriteComponentFactory : public ComponentFactory
{
  Component *operator()(const component_factory_data& data) override;
};
