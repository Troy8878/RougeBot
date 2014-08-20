/*********************************
 * SpriteComponent.cpp
 * Jake Robsahm
 * Created 2014/08/19
 *********************************/

#include "Common.h"
#include "SpriteComponent.h"
#include "StandardShapes.h"
#include "Game.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

SpriteComponentFactory SpriteComponent::factory;

// ----------------------------------------------------------------------------

// Constructor if only one Texture/Sprite is needed (unanimated)
SpriteComponent::SpriteComponent(Texture2D texture)
  : SpriteComponent(std::vector<Texture2D>{texture})
{
}

// ----------------------------------------------------------------------------

// Constructor if several Textures/Sprites are needed (animated)
SpriteComponent::SpriteComponent(const std::vector<Texture2D>& textures)
  : _textures(textures)
{
  RenderCamera = nullptr;
  UnitSquare = GetSpriteModel();
}

// ----------------------------------------------------------------------------

// Destructor for the SpriteComponent
SpriteComponent::~SpriteComponent()
{
}

// ----------------------------------------------------------------------------

void SpriteComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  static Events::EventId drawId("draw");
  owner->AddEvent(this, drawId, &SpriteComponent::Draw);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Draw(Events::EventMessage&)
{
  UnitSquare->texture = _textures[0];
  UnitSquare->Draw(DirectX::XMMatrixIdentity());
}

// ----------------------------------------------------------------------------

Model *SpriteComponent::GetSpriteModel()
{
  THREAD_EXCLUSIVE_SCOPE;

  static Model *unitSquare = nullptr;
  
  if (unitSquare)
    return unitSquare;

  unitSquare = Shapes::MakeRectangle(GetGame()->GameDevice->Device, {1, 1});
  unitSquare->shader = RegisteredShaders["Textured"];
  
  return unitSquare;
}

// ----------------------------------------------------------------------------

SpriteComponentFactory::SpriteComponentFactory()
  : allocator(sizeof(SpriteComponent))
{
}

// ----------------------------------------------------------------------------

Component *SpriteComponentFactory::operator()(void *memory, component_factory_data& data)
{
  auto texture = TextureManager::Instance.LoadTexture(data["texture"]);
  SpriteComponent *component = new (memory) SpriteComponent(texture);

  return component;
}

// ----------------------------------------------------------------------------
