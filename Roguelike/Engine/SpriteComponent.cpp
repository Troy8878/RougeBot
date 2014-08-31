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
SpriteComponent::SpriteComponent(Texture2D texture, 
                                 Shader *shader, RenderSet *set)
  : SpriteComponent(std::vector<Texture2D>{texture}, shader, set)
{
}

// ----------------------------------------------------------------------------

// Constructor if several Textures/Sprites are needed (animated)
SpriteComponent::SpriteComponent(const std::vector<Texture2D>& textures, 
                                 Shader *shader, RenderSet *set)
  : _textures(textures), renderTarget(set)
{
  UnitSquare = GetSpriteModel();
  ModelShader = shader;
}

// ----------------------------------------------------------------------------

// Destructor for the SpriteComponent
SpriteComponent::~SpriteComponent()
{
  renderTarget->RemoveDrawable(this);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  Transform = Owner->GetComponent<TransformComponent>("TransformComponent");
  renderTarget->AddDrawable(this, ModelShader);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Draw()
{
  auto& transform = Transform->Matrix;

  UnitSquare->shader = ModelShader;
  UnitSquare->texture = _textures[TextureIndex];
  UnitSquare->Draw(transform.get());
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

Component *SpriteComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  auto texture = TextureManager::Instance.LoadTexture(data["texture"]);
  auto shader = RegisteredShaders[data["shader"]];
  auto set = RenderGroup::Instance.GetSet(data["render_target"]);

  SpriteComponent *component = new (memory) SpriteComponent(texture, shader, set);

  return component;
}

// ----------------------------------------------------------------------------
