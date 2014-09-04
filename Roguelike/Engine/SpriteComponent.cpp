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
  : _textures(textures), renderTarget(set), TextureIndex(0)
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
  auto shader = RegisteredShaders[map_fetch(data, "shader", "Textured")];
  auto set = RenderGroup::Instance.GetSet(data["render_target"]);

  SpriteComponent *component;

  auto textures_it = data.find("textures");
  if (textures_it != data.end())
  {
    auto jtextures = json::value::parse(textures_it->second);
    auto texture_names = jtextures.as_array_of<json::value::string_t>();

    std::vector<Texture2D> textures;
    textures.reserve(texture_names.size());
    for (auto& name : texture_names)
    {
      textures.push_back(TextureManager::Instance.LoadTexture(name));
    }

    component = new (memory) SpriteComponent(textures, shader, set);
  }
  else
  {
    auto texture = TextureManager::Instance.LoadTexture(data["texture"]);
    component = new (memory) SpriteComponent(texture, shader, set);
  }

  return component;
}

// ----------------------------------------------------------------------------

// Constrctor for ruby Components::SpriteComponent
mrb_value rb_sprite_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value spriteWrapper;
  mrb_get_args(mrb, "o", &spriteWrapper);

  static mrb_sym wrapperSym = mrb_intern_cstr(mrb, "comp_ptr");
  mrb_iv_set(mrb, self, wrapperSym, spriteWrapper);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

SpriteComponent *rb_help_getSpriteComponent(mrb_state *mrb, mrb_value self)
{
  static mrb_sym wrapperSym = mrb_intern_cstr(mrb, "comp_ptr");
  mrb_value spriteWrapper = mrb_iv_get(mrb, self, wrapperSym);

  ruby::ruby_engine engine{mrb};

  return static_cast<SpriteComponent *>(engine.unwrap_native_ptr(spriteWrapper));
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

ruby::ruby_value SpriteComponent::GetRubyWrapper()
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
