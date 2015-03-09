/*********************************
 * CustomModelComponent.cpp
 * Connor Hilarides
 * Created 2014/09/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "CustomModelComponent.h"
#include "Shader.h"
#include "TextureComponent.h"

// ----------------------------------------------------------------------------

CustomModelComponentFactory CustomModelComponent::factory;

// ----------------------------------------------------------------------------

CustomModelComponent::CustomModelComponent(Shader *shader, RenderSet *set)
  : _ModelShader(shader), renderTarget(set), customModel(nullptr)
{
}

// ----------------------------------------------------------------------------

CustomModelComponent::~CustomModelComponent()
{
  delete CustomModel;
}

// ----------------------------------------------------------------------------

void CustomModelComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  texture = static_cast<TextureComponent *>(Owner->GetComponent("TextureComponent"));
  renderTarget->AddDrawable(this, ModelShader);
}

// ----------------------------------------------------------------------------

void CustomModelComponent::Draw()
{
  if (CustomModel)
  {
    CustomModel->Draw(Owner->Transform);
  }
}

// ----------------------------------------------------------------------------

void CustomModelComponent::_SetCustomModel(Model *model)
{
  model->shader = ModelShader;
  customModel = model;

  if (texture)
    customModel->texture = texture->Textures[0];
}

// ----------------------------------------------------------------------------

CustomModelComponentFactory::CustomModelComponentFactory()
  : allocator(sizeof(CustomModelComponent))
{
}

// ----------------------------------------------------------------------------

auto CustomModelComponentFactory::CreateObject(void *memory, component_factory_data &data)
-> Component *
{
  auto shader = RegisteredShaders[map_fetch(data, "shader", "Basic").as_string()];
  auto set = RenderGroup::Instance.GetSet(data["render_target"].as_string());

  return new(memory) CustomModelComponent(shader, set);
}

// ----------------------------------------------------------------------------