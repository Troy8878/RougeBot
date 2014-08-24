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
#include "TransformComponent.h"

// ----------------------------------------------------------------------------

SpriteComponentFactory SpriteComponent::factory;

// ----------------------------------------------------------------------------

// Constructor if only one Texture/Sprite is needed (unanimated)
SpriteComponent::SpriteComponent(Texture2D texture, Shader *shader)
  : SpriteComponent(std::vector<Texture2D>{texture}, shader)
{
}

// ----------------------------------------------------------------------------

// Constructor if several Textures/Sprites are needed (animated)
SpriteComponent::SpriteComponent(const std::vector<Texture2D>& textures, Shader *shader)
  : _textures(textures)
{
  UnitSquare = GetSpriteModel();
  ModelShader = shader;
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

  Transform = Owner->GetComponent<TransformComponent>("TransformComponent");
}

// ----------------------------------------------------------------------------

void SpriteComponent::Draw()
{
  auto& trans = Transform->Matrix;

  UnitSquare->shader = ModelShader;
  UnitSquare->texture = _textures[0];
  UnitSquare->Draw(trans.get());
}

// ----------------------------------------------------------------------------

Model *SpriteComponent::GetSpriteModel()
{
  THREAD_EXCLUSIVE_SCOPE;

  static Model *unitSquare = nullptr;
  
  if (unitSquare)
    return unitSquare;

  unitSquare = Shapes::MakeRectangle(GetGame()->GameDevice->Device, {1, 1});
  return unitSquare;
}

// ----------------------------------------------------------------------------

SpriteComponentFactory::SpriteComponentFactory()
  : allocator(sizeof(SpriteComponent))
{
}

// ----------------------------------------------------------------------------

Component *SpriteComponentFactory::operator()(
  void *memory, component_factory_data& data)
{
  auto texture = TextureManager::Instance.LoadTexture(data["texture"]);
  auto shader = RegisteredShaders[data["shader"]];
  auto set = RenderGroup::Instance.GetSet(data["render_target"]);

  SpriteComponent *component = new (memory) SpriteComponent(texture, shader);
  set->AddDrawable(component, shader);

  return component;
}

// ----------------------------------------------------------------------------
