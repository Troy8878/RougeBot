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
#include "EventTypes.h"
#include "Event.h"
#include "Entity.h"

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

  DEF_EVENT_ID(set_model_tex);
  Owner->AddEvent(this, set_model_tex, &CustomModelComponent::OnSetTexture);
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

void CustomModelComponent::OnSetTexture(Events::EventMessage &e)
{
  auto rdata = dynamic_cast<Events::RubyEvent *>(e.Data);
  if (rdata && mrb_fixnum_p(rdata->ruby_obj))
  {
    auto index = mrb_fixnum(rdata->ruby_obj);
    TextureIndex = index;

    if (texture && customModel)
      customModel->texture = texture->Textures[TextureIndex % texture->TextureCount];
  }
}

// ----------------------------------------------------------------------------

void CustomModelComponent::_SetCustomModel(Model *model)
{
  model->shader = ModelShader;
  customModel = model;

  if (texture)
    customModel->texture = texture->Textures[TextureIndex % texture->TextureCount];
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