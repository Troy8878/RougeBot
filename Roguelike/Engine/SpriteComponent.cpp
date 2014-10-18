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
#include "mruby/variable.h"
#include "json/json.h"

// ----------------------------------------------------------------------------

SpriteComponentFactory SpriteComponent::factory;

// ----------------------------------------------------------------------------

// Constructor if several Textures/Sprites are needed (animated)
SpriteComponent::SpriteComponent(Shader *shader, RenderSet *set)
  : renderTarget(set), TextureIndex(0)
{
  UnitSquare = GetSpriteModel();
  ModelShader = shader;
}

// ----------------------------------------------------------------------------

// Destructor for the SpriteComponent
SpriteComponent::~SpriteComponent()
{
  if (renderTarget)
    renderTarget->RemoveDrawable(this);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  _texture = (TextureComponent *) Owner->GetComponent("TextureComponent");
  renderTarget->AddDrawable(this, ModelShader);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Draw()
{
  auto transform = Owner->Transform.get();
  
  if (_texture)
    UnitSquare->texture = _texture->Textures[TextureIndex];
  else
    UnitSquare->texture = Texture2D();
  
  UnitSquare->shader = ModelShader;
  UnitSquare->Draw(transform);
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
  auto shader_name = map_fetch(data, "shader", "Textured").as_string();
  auto shader = RegisteredShaders[shader_name];

  if (shader == nullptr)
    throw string_exception("Shader '" + shader_name + 
                           "' could not be found while initializing SpriteComponent!");

  auto set_name = data["render_target"].as_string();
  auto set = RenderGroup::Instance.GetSet(set_name);

  if (set == nullptr)
    throw string_exception("Render Target '" + set_name + 
                           "' could not be found while initializing SpriteComponent!");

  return new (memory) SpriteComponent(shader, set);
}

// ----------------------------------------------------------------------------

// Constrctor for ruby Components::SpriteComponent
mrb_value rb_sprite_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value spriteWrapper;
  mrb_get_args(mrb, "o", &spriteWrapper);

  ruby::save_native_ptr(mrb, self, mrb_ptr(spriteWrapper));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

SpriteComponent *rb_help_getSpriteComponent(mrb_state *mrb, mrb_value self)
{
  return ruby::read_native_ptr<SpriteComponent>(mrb, self);
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_get_textureindex(mrb_state *mrb, mrb_value self)
{
  auto sprite = rb_help_getSpriteComponent(mrb, self);
  
  return mrb_fixnum_value(sprite->TextureIndex);
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_set_textureindex(mrb_state *mrb, mrb_value self)
{
  auto sprite = rb_help_getSpriteComponent(mrb, self);
  
  mrb_int newIndex;
  mrb_get_args(mrb, "i", &newIndex);

  sprite->TextureIndex = newIndex;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_get_texturecount(mrb_state *mrb, mrb_value self)
{
  auto sprite = rb_help_getSpriteComponent(mrb, self);
  
  return mrb_fixnum_value(sprite->TextureCount);
}

// ----------------------------------------------------------------------------

mrb_value SpriteComponent::GetRubyWrapper()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool initialized = false;
  static ruby::ruby_class component;

  if (!initialized)
  {
    auto module = GetComponentRModule();
    auto base_class = GetComponentRClass();
    component = module.define_class("SpriteComponent", base_class);

    component.define_method("initialize", rb_sprite_initialize, ARGS_REQ(1));
    component.define_method("texture_index", rb_sprite_get_textureindex, ARGS_NONE());
    component.define_method("texture_index=", rb_sprite_set_textureindex, ARGS_REQ(1));
    component.define_method("texture_count", rb_sprite_get_texturecount, ARGS_NONE());

    initialized = true;
  }

  auto compwrap = ruby::ruby_engine::global_engine->wrap_native_ptr(this);
  return component.new_inst(compwrap).silent_reset();
}
