/*********************************
 * CustomModelComponent.cpp
 * Connor Hilarides
 * Created 2014/09/09
 *********************************/

#include "Common.h"
#include "CustomModelComponent.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

CustomModelComponentFactory CustomModelComponent::factory;

// ----------------------------------------------------------------------------

CustomModelComponent::CustomModelComponent(Shader *shader, RenderSet *set)
  : _ModelShader(shader), renderTarget(set)
{
}

// ----------------------------------------------------------------------------

CustomModelComponent::~CustomModelComponent()
{
  delete CustomModel;
}

// ----------------------------------------------------------------------------

void CustomModelComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

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
}

// ----------------------------------------------------------------------------

CustomModelComponentFactory::CustomModelComponentFactory()
  : allocator(sizeof(CustomModelComponent))
{
}

// ----------------------------------------------------------------------------

auto CustomModelComponentFactory::CreateObject(void *memory, component_factory_data& data) 
  -> Component *
{
  auto shader = RegisteredShaders[map_fetch(data, "shader", "Basic")];
  auto set = RenderGroup::Instance.GetSet(data["render_target"]);

  return new (memory) CustomModelComponent(shader, set);
}

// ----------------------------------------------------------------------------
