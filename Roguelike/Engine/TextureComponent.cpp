/*********************************
 * TextureComponent.h
 * Connor Hilarides
 * Created 2014/09/14
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "TextureComponent.h"
#include "Texture.h"
#include "TextureZip.h"

#pragma warning (disable: 4127)

// ----------------------------------------------------------------------------

TextureComponentFactory TextureComponent::factory;

// ----------------------------------------------------------------------------

TextureComponent::TextureComponent()
{
}

// ----------------------------------------------------------------------------

void TextureComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);
}

// ----------------------------------------------------------------------------

void TextureComponent::AddTexture(json::value definition)
{
  textures.push_back(Texture2D::LoadTextureDefinition(definition));
}

// ----------------------------------------------------------------------------

void TextureComponent::RemoveTexture(size_t index)
{
  if (index >= textures.size())
    return;
  textures.erase(textures.begin() + index);
}

// ----------------------------------------------------------------------------

TextureComponentFactory::TextureComponentFactory()
  : allocator(sizeof(TextureComponent))
{
}

// ----------------------------------------------------------------------------

Component *TextureComponentFactory::CreateObject(
  void *memory, component_factory_data &data)
{
  auto component = new(memory) TextureComponent();

  auto &jtextures = data["textures"];
  if (jtextures.is(json::json_type::jarray))
  {
    auto &textures = jtextures.as_array();

    for (auto &Textureef : textures)
    {
      component->AddTexture(Textureef);
    }
  }

  return component;
}

// ----------------------------------------------------------------------------

mrb_data_type mrb_texturecomp_dt;

static void mrb_texture_gem_init(mrb_state *mrb, RClass *module, RClass *base);

static mrb_value mrb_texture_new(mrb_state *mrb, TextureComponent *comp);

static void mrb_texture_free(mrb_state *, void *)
{
}

static mrb_value mrb_texture_at(mrb_state *mrb, mrb_value self);
static mrb_value mrb_texture_add(mrb_state *mrb, mrb_value self);
static mrb_value mrb_texture_remove(mrb_state *mrb, mrb_value self);

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

  mrb_define_method(mrb, texture, "[]", mrb_texture_at, MRB_ARGS_NONE());
  mrb_define_method(
    mrb, texture, "length",
    ruby::data_getter_access_integer<
      TextureComponent, &mrb_texturecomp_dt,
      size_t, &TextureComponent::_GetTextureCount>,
    MRB_ARGS_NONE());

  mrb_define_method(mrb, texture, "add", mrb_texture_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, texture, "remove", mrb_texture_remove, MRB_ARGS_REQ(1));

  mrb_define_alias(mrb, texture, "size", "length");
  mrb_define_alias(mrb, texture, "count", "length");

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

static mrb_value mrb_texture_at(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  auto &comp = *ruby::data_get<TextureComponent>(mrb, self);

  return comp.Textures[static_cast<size_t>(index)].RubyWrapper;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_texture_add(mrb_state *mrb, mrb_value self)
{
  auto &comp = *ruby::data_get<TextureComponent>(mrb, self);

  mrb_value data;
  mrb_get_args(mrb, "o", &data);

  try
  {
    auto json = ruby::ruby_engine{mrb}.value_to_json(data);
    comp.AddTexture(json);

    return mrb_fixnum_value(static_cast<mrb_int>(comp.TextureCount) - 1);
  }
  catch (std::exception &ex)
  {
    mrb_raise(mrb, E_RUNTIME_ERROR, ex.what());
  }
}

// ----------------------------------------------------------------------------

static mrb_value mrb_texture_remove(mrb_state *mrb, mrb_value self)
{
  auto &comp = *ruby::data_get<TextureComponent>(mrb, self);

  mrb_value item;
  mrb_get_args(mrb, "o", &item);

  if (mrb_fixnum_p(item))
  {
    size_t index = static_cast<size_t>(mrb_fixnum(item));
    if (index >= comp.TextureCount)
      return mrb_false_value();

    comp.RemoveTexture(index);
    return mrb_true_value();
  }
  if (mrb_string_p(item))
  {
    auto name = mrb_str_to_stdstring(item);
    auto &textures = comp.Textures;
    auto it = std::find_if(
      textures.begin(), textures.end(),
      [&name](const Texture2D &tex)
      {
        return tex.Name == name;
      });

    if (it != textures.end())
    {
      size_t index = std::distance(textures.begin(), it);
      comp.RemoveTexture(index);
      return mrb_true_value();
    }
    return mrb_false_value();
  }
  mrb_raise(mrb, E_TYPE_ERROR,
            "TextureComponent#remove must take "
            "either a Fixnum(index) or String(name)");
}

// ----------------------------------------------------------------------------