/*********************************
 * SpriteComponent.cpp
 * Jake Robsahm
 * Created 2014/08/19
 *********************************/

#include "Common.h"
#include "SpriteComponent.h"
#include "StandardShapes.h"
#include "Game.h"

SpriteComponentFactory SpriteComponent::factory;

// Constructor if only one Texture/Sprite is needed (unanimated)
SpriteComponent::SpriteComponent(Texture2D texture)
  : SpriteComponent(std::vector<Texture2D>{{texture}})
{

}

// Constructor if several Textures/Sprites are needed (animated)
SpriteComponent::SpriteComponent(const std::vector<Texture2D>& textures)
  : _textures(textures)
{

}

// Destructor for the SpriteComponent
SpriteComponent::~SpriteComponent()
{
  
}

Model *SpriteComponent::GetSpriteModel()
{
  THREAD_EXCLUSIVE_SCOPE;

  static Model *unitSquare = nullptr;
  
  if (unitSquare)
    return unitSquare;

  unitSquare = Shapes::MakeRectangle(GetGame()->GameDevice->Device, {1, 1});
  
  return unitSquare;
}
