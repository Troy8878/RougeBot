/*********************************
 * SpriteComponent.cpp
 * Jake Robsahm, Leonardo Saikali
 * Created 2014/08/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "SpriteComponent.h"
#include "StandardShapes.h"
#include "Game.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "mruby/variable.h"
#include "json/json.h"
#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

SpriteComponentFactory SpriteComponent::factory;

// ----------------------------------------------------------------------------

// Constructor if several Textures/Sprites are needed (animated)
SpriteComponent::SpriteComponent(Shader *shader, RenderSet *set)
  : renderTarget(set), _TextureIndex(0), _Visible(true)
{
  UnitSquare = GetSpriteModel();
  ModelShader = shader;
  Tint = math::Vector{1,1,1,1};
}

// ----------------------------------------------------------------------------

// Destructor for the SpriteComponent
SpriteComponent::~SpriteComponent()
{
  if (renderTarget)
    renderTarget->RemoveDrawable(this);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  _texture = static_cast<TextureComponent *>(Owner->GetComponent("TextureComponent"));
  renderTarget->AddDrawable(this, ModelShader);

  DEF_EVENT_ID(sprite_hide);
  DEF_EVENT_ID(sprite_show);
  Owner->AddEvent(this, sprite_hide, &SpriteComponent::SpriteHide);
  Owner->AddEvent(this, sprite_show, &SpriteComponent::SpriteShow);
}

// ----------------------------------------------------------------------------

void SpriteComponent::Draw()
{
  if (!Visible)
    return;

  auto transform = Owner->Transform.get();
  auto &textures = _texture->Textures;

  if (_texture && TextureCount)
    UnitSquare->texture = textures[TextureIndex % TextureCount];
  else
    UnitSquare->texture = Texture2D();

  UnitSquare->shader = ModelShader;
  UnitSquare->tint = Tint;
  UnitSquare->tintTexture = TintTexture;
  UnitSquare->Draw(transform);
}

// ----------------------------------------------------------------------------

void SpriteComponent::SpriteHide(Events::EventMessage &)
{
  Visible = false;
}

// ----------------------------------------------------------------------------

void SpriteComponent::SpriteShow(Events::EventMessage &)
{
  Visible = true;
}

// ----------------------------------------------------------------------------

Model *SpriteComponent::GetSpriteModel()
{
  THREAD_EXCLUSIVE_SCOPE;

  static Model *unitSquare = nullptr;

  if (unitSquare)
    return unitSquare;

  const UINT segments = 4;
  const UINT vertex_count = segments * segments;
  const UINT index_count = (segments - 1) * (segments - 1) * 6;

  TexturedVertex vertices[vertex_count];
  UINT indices[index_count];

  for (UINT y = 0; y < segments; ++y)
  {
    for (UINT x = 0; x < segments; ++x)
    {
      auto xprog = x * (1.0f / (segments - 1));
      auto yprog = y * (1.0f / (segments - 1));

      auto &vertex = vertices[y * segments + x];
      vertex.position.x = -0.5f + xprog;
      vertex.position.y = 0.5f - yprog;
      vertex.texture.x = xprog;
      vertex.texture.y = yprog;
      vertex.color = math::Vector{1,1,1,1};
    }
  }

  for (UINT y = 0; y < segments - 1; ++y)
  {
    for (UINT x = 0; x < segments - 1; ++x)
    {
      UINT base = (y * (segments - 1) + x) * 6;

      UINT vtl = (y) * segments + (x);
      UINT vtr = (y) * segments + (x + 1);
      UINT vbl = (y + 1) * segments + (x);
      UINT vbr = (y + 1) * segments + (x + 1);

      (vtl , vtr , vbl , vbr);

      indices[base + 0] = vtl;
      indices[base + 1] = vbl;
      indices[base + 2] = vbr;
      indices[base + 3] = vtl;
      indices[base + 4] = vbr;
      indices[base + 5] = vtr;
    }
  }

  unitSquare = new Model(GetGame()->GameDevice->Device, vertices, indices);
  return unitSquare;
}

// ----------------------------------------------------------------------------

SpriteComponentFactory::SpriteComponentFactory()
  : allocator(sizeof(SpriteComponent))
{
}

// ----------------------------------------------------------------------------

Component *SpriteComponentFactory::CreateObject(
  void *memory, component_factory_data &data)
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

  auto *sprite = new (memory) SpriteComponent(shader, set);

  auto tintColor = map_fetch(data, "tint", nullptr);
  sprite->Tint = JsonToColor(tintColor);

  return sprite;
}

// ----------------------------------------------------------------------------


static RClass *cbase;

static mrb_data_type mrb_spritecomp_data_type;

static mrb_value mrb_spritecomp_new(mrb_state *mrb, SpriteComponent *comp);

static void mrb_spritecomp_free(mrb_state *, void *)
{
}

static mrb_value rb_sprite_initialize(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_textureindex(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_set_textureindex(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_texturecount(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_visible(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_set_visible(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_tint(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_set_tint(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_tint_tex(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_set_tint_tex(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static void mrb_spritecomp_gem_init(mrb_state *mrb)
{
  mrb_spritecomp_data_type.dfree = mrb_spritecomp_free;
  mrb_spritecomp_data_type.struct_name = typeid(SpriteComponent).name();

  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_define_class_under(mrb, rmod, "SpriteComponent", cbase);

  mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_ANY());

  mrb_define_method(mrb, rclass, "initialize", rb_sprite_initialize, ARGS_REQ(1));

  mrb_define_method(mrb, rclass, "texture_count", rb_sprite_get_texturecount, ARGS_NONE());
  mrb_define_method(mrb, rclass, "texture_index", rb_sprite_get_textureindex, ARGS_NONE());
  mrb_define_method(mrb, rclass, "texture_index=", rb_sprite_set_textureindex, ARGS_REQ(1));
  comp_add_property(mrb, rclass, "texture_index", "int", true);

  mrb_define_method(mrb, rclass, "visible", rb_sprite_get_visible, ARGS_NONE());
  mrb_define_method(mrb, rclass, "visible=", rb_sprite_set_visible, ARGS_REQ(1));
  comp_add_property(mrb, rclass, "visible", "bool", true);

  mrb_define_method(mrb, rclass, "tint", rb_sprite_get_tint, ARGS_NONE());
  mrb_define_method(mrb, rclass, "tint=", rb_sprite_set_tint, ARGS_REQ(1));
  comp_add_property(mrb, rclass, "tint", "color");

  mrb_define_method(mrb, rclass, "tint_texture", rb_sprite_get_tint_tex, ARGS_NONE());
  mrb_define_method(mrb, rclass, "tint_texture=", rb_sprite_set_tint_tex, ARGS_REQ(1));
}


// ----------------------------------------------------------------------------

mrb_value SpriteComponent::GetRubyWrapper()
{
  RUN_ONCE(cbase = Component::GetComponentRClass(),
    mrb_spritecomp_gem_init(*mrb_inst));

  return mrb_spritecomp_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_spritecomp_new(mrb_state *mrb, SpriteComponent *comp)
{
  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_class_get_under(mrb, rmod, "SpriteComponent");

  auto obj = mrb_data_object_alloc(mrb, rclass, comp, &mrb_spritecomp_data_type);
  return mrb_obj_value(obj);
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

mrb_value rb_sprite_get_textureindex(mrb_state *mrb, mrb_value self)
{
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);

  return mrb_fixnum_value(sprite->TextureIndex);
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_set_textureindex(mrb_state *mrb, mrb_value self)
{
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);

  mrb_int newIndex;
  mrb_get_args(mrb, "i", &newIndex);

  sprite->TextureIndex = newIndex;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_get_texturecount(mrb_state *mrb, mrb_value self)
{
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);

  return mrb_fixnum_value(sprite->TextureCount);
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_get_visible(mrb_state *mrb, mrb_value self)
{
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);
  return mrb_bool_value(sprite->Visible);
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_set_visible(mrb_state *mrb, mrb_value self)
{
  mrb_bool value;
  mrb_get_args(mrb, "b", &value);
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);

  sprite->Visible = !!value;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_sprite_get_tint(mrb_state *mrb, mrb_value self)
{
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);
  return ruby::wrap_memory_vector(&sprite->Tint);
}

// ----------------------------------------------------------------------------

static mrb_value rb_sprite_set_tint(mrb_state *mrb, mrb_value self)
{
  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);

  if (mrb_string_p(value))
  {
    sprite->Tint = StringToColor(mrb_str_to_stdstring(value));
  }
  else
  {
    sprite->Tint = ruby::get_ruby_vector(value);
  }

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_sprite_get_tint_tex(mrb_state *mrb, mrb_value self)
{
  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);
  return sprite->TintTexture.RubyWrapper;
}

// ----------------------------------------------------------------------------

static mrb_value rb_sprite_set_tint_tex(mrb_state *mrb, mrb_value self)
{
  mrb_value param;
  mrb_get_args(mrb, "o", &param);

  auto sprite = ruby::data_get<SpriteComponent>(mrb, self);
  auto tex = *ruby::data_get<Texture2D>(mrb, param);

  sprite->TintTexture = tex;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------