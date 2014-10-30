/*********************************
 * SpriteComponent.cpp
 * Jake Robsahm, Leonardo Saikali
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
  : renderTarget(set), _TextureIndex(0), _Visible(true)
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
  if (!Visible)
    return;

  auto transform = Owner->Transform.get();
  auto& textures = _texture->Textures;

  if (_texture && TextureCount)
    UnitSquare->texture = textures[TextureIndex % TextureCount];
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


static RClass *cbase;

static mrb_data_type mrb_spritecomp_data_type;

static mrb_value mrb_spritecomp_new(mrb_state *mrb, SpriteComponent *comp);
static void mrb_spritecomp_free(mrb_state *, void *) {}

static mrb_value rb_sprite_initialize(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_textureindex(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_set_textureindex(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_texturecount(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_get_visible(mrb_state *mrb, mrb_value self);
static mrb_value rb_sprite_set_visible(mrb_state *mrb, mrb_value self);


// ----------------------------------------------------------------------------

static void mrb_spritecomp_gem_init(mrb_state *mrb)
{
  mrb_spritecomp_data_type.dfree = mrb_spritecomp_free;
  mrb_spritecomp_data_type.struct_name = "SpriteComponent";

  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_define_class_under(mrb, rmod, "SpriteComponent", cbase);

  mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_ANY());

  mrb_define_method(mrb, rclass, "initialize", rb_sprite_initialize, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "texture_index", rb_sprite_get_textureindex, ARGS_NONE());
  mrb_define_method(mrb, rclass, "texture_index=", rb_sprite_set_textureindex, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "texture_count", rb_sprite_get_texturecount, ARGS_NONE());
  mrb_define_method(mrb, rclass, "visible", rb_sprite_get_visible, ARGS_NONE());
  mrb_define_method(mrb, rclass, "visible=", rb_sprite_set_visible, ARGS_REQ(1));
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

SpriteComponent *rb_help_getSpriteComponent(mrb_state *mrb, mrb_value self)
{
  return (SpriteComponent *)mrb_data_get_ptr(mrb, self, &mrb_spritecomp_data_type);
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

mrb_value rb_sprite_get_visible(mrb_state *mrb, mrb_value self)
{
  auto sprite = rb_help_getSpriteComponent(mrb, self);
  return mrb_bool_value(sprite->Visible);
}

// ----------------------------------------------------------------------------

mrb_value rb_sprite_set_visible(mrb_state *mrb, mrb_value self)
{
  mrb_bool value;
  mrb_get_args(mrb, "b", &value);
  auto sprite = rb_help_getSpriteComponent(mrb, self);

  sprite->Visible = !!value;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
