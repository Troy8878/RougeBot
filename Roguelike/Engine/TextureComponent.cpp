/*********************************
 * TextureComponent.h
 * Connor Hilarides
 * Created 2014/09/14
 *********************************/

#include "Common.h"
#include "TextureComponent.h"
#include "Texture.h"
#include "TextureZip.h"

// ----------------------------------------------------------------------------

TextureComponentFactory TextureComponent::factory;

// ----------------------------------------------------------------------------

TextureComponent::TextureComponent()
{
}

// ----------------------------------------------------------------------------

void TextureComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);
}

// ----------------------------------------------------------------------------

void TextureComponent::AddTexture(json::value Textureef)
{
  static std::unordered_map<std::string, Texture2D(*)(json::value)> constructors =
  {
    {"texture", TextureComponent::ConstructTexture},
    {"zipped", TextureComponent::ConstructZipped}
  };

  if (Textureef.is(json::json_type::jobject))
  {
    auto& obj = Textureef.as_object();

    assert(obj.size() == 1); // THERE CAN ONLY BE ONE!!!!
    auto& pair = *obj.begin();

    textures.push_back(constructors[pair.first](pair.second));
  }
  else
  {
    assert(Textureef.is(json::json_type::jstring));
    textures.push_back(ConstructTexture(Textureef));
  }
}

// ----------------------------------------------------------------------------

void TextureComponent::RemoveTexture(size_t index)
{
  textures.erase(textures.begin() + index);
}

// ----------------------------------------------------------------------------

Texture2D TextureComponent::ConstructTexture(json::value definition)
{
  return TextureManager::Instance.LoadTexture(definition.as_string());
}

// ----------------------------------------------------------------------------

Texture2D TextureComponent::ConstructZipped(json::value definition)
{
  assert(definition.is_array_of<json::value::string_t>());

  TextureZip zip(definition.as_array_of<json::value::string_t>());
  return Texture2D::FromTextureZip(zip);
}

// ----------------------------------------------------------------------------

TextureComponentFactory::TextureComponentFactory()
  : allocator(sizeof(TextureComponent))
{
}

// ----------------------------------------------------------------------------

Component *TextureComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  auto component = new (memory) TextureComponent();

  auto& jtextures = data["textures"];
  if(jtextures.is(json::json_type::jarray))
  {
    auto& textures = jtextures.as_array();

    for (auto& Textureef : textures)
    {
      component->AddTexture(Textureef);
    }
  }

  return component;
}

// ----------------------------------------------------------------------------

static mrb_data_type mrb_texturecomp_dt;

static void mrb_texture_gem_init(mrb_state *mrb, RClass *module, RClass *base);

static mrb_value mrb_texture_new(mrb_state *mrb, TextureComponent *comp);
static void mrb_texture_free(mrb_state *, void *) {}

static mrb_value mrb_texture_each(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

mrb_value TextureComponent::GetRubyWrapper()
{
  RUN_ONCE(mrb_texture_gem_init(*mrb_inst, GetComponentRModule(), GetComponentRClass()));

  return mrb_texture_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

static void mrb_texture_gem_init(mrb_state *mrb, RClass *module, RClass *base)
{
  mrb_texturecomp_dt.dfree = mrb_texture_free;
  mrb_texturecomp_dt.struct_name = typeid(TextureComponent).name();
  
  auto texture = mrb_define_class_under(mrb, module, "TextureComponent", base);

  mrb_define_class_method(mrb, texture, "new", mrb_nop, MRB_ARGS_NONE());

  mrb_define_method(mrb, texture, "each", mrb_texture_each, MRB_ARGS_BLOCK());

  mrb_funcall(mrb, mrb_obj_value(texture), "include", 1,
              mrb_obj_value(mrb_module_get(mrb, "Enumerable")));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_texture_new(mrb_state *mrb, TextureComponent *comp)
{
  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_class_get_under(mrb, rmod, "TextureComponent");

  auto obj = mrb_data_object_alloc(mrb, rclass, comp, &mrb_texturecomp_dt);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_texture_each(mrb_state *mrb, mrb_value self)
{
  mrb_value block;
  mrb_get_args(mrb, "|&", &block);

  if (mrb_nil_p(block))
  {
    static const auto to_enum = mrb_intern_lit(mrb, "to_enum");
    static const auto each = mrb_symbol_value(mrb_intern_lit(mrb, "each"));

    return mrb_funcall_argv(mrb, self, to_enum, 1, &each);
  }

  auto& comp = *(TextureComponent *) mrb_data_get_ptr(mrb, self, &mrb_texturecomp_dt);

  for (auto& texture : comp.Textures)
  {
    mrb_yield(mrb, block, texture.RubyWrapper);
  }

  return self;
}

// ----------------------------------------------------------------------------

