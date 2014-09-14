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
  assert(jtextures.is(json::json_type::jarray));
  auto textures = jtextures.as_array();

  for (auto& Textureef : textures)
  {
    component->AddTexture(Textureef);
  }

  return component;
}

// ----------------------------------------------------------------------------

mrb_value TextureComponent::GetRubyWrapper()
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
