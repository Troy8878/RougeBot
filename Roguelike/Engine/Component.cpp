/*********************************
 * Component.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

ComponentManager ComponentManager::Instance;

// ----------------------------------------------------------------------------

Component::Component()
  : _Owner(nullptr)
{
}

// ----------------------------------------------------------------------------

void Component::Initialize(Entity *owner, const std::string& name)
{
  Owner = owner;
  Name = name;
}

// ----------------------------------------------------------------------------

void Component::Cleanup()
{
}

// ----------------------------------------------------------------------------

ComponentManager::ComponentManager()
{
}

// ----------------------------------------------------------------------------

void ComponentManager::RegisterComponent(const ComponentRegistration& registration)
{
  ComponentRegistrations[registration.componentName] = registration;
}

// ----------------------------------------------------------------------------

Component *ComponentManager::InstantiateComponent(const std::string& compName, 
                                                  component_factory_data& data)
{
  auto& regs = ComponentRegistrations;
  auto it = regs.find(compName);
  if (it == regs.end())
    throw string_exception("Component was requested to be created, "
                           "but is not a registered component (" + compName + ")");
  auto& reg = it->second;

  auto *memory = reg.Allocator.Allocate();
  auto *component = reg.Factory.CreateObject(memory, data);

  return component;
}

// ----------------------------------------------------------------------------

void ComponentManager::ReleaseComponent(Component *component)
{
  auto& reg = ComponentRegistrations[component->Name];
  
  component->~Component();
  reg.Allocator.Free(component);
}

// ----------------------------------------------------------------------------

auto ComponentManager::_GetComponentRegistrations() -> component_map&
{
  static component_map registrations;
  return registrations;
}

// ----------------------------------------------------------------------------

static void RegisterStaticComponents();

void RegisterEngineComponents()
{
  auto& rbengine = *ruby::ruby_engine::global_engine;
  Component::GetComponentRClass();

  auto scriptResCont = GetGame()->Respack["Scripts"];
  RELEASE_AFTER_SCOPE(scriptResCont);

  auto scriptResources = scriptResCont->Resources;

  // Ruby Support Classes
  std::regex supportPattern{"Support/(.*)\\.rb", std::regex::icase};
  for (auto& resource : scriptResources)
  {
    if (std::regex_match(resource, supportPattern))
    {
      rbengine.evaluate_asset(resource);
    }
  }

  // Ruby Components
  std::regex componentPattern{"Components/(.*)\\.rb", std::regex::icase};
  for (auto& resource : scriptResources)
  {
    if (std::regex_match(resource, componentPattern))
    {
      rbengine.evaluate_asset(resource);
    }
  }

  RegisterStaticComponents();
}

// ----------------------------------------------------------------------------

#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "CustomModelComponent.h"
#include "TextureComponent.h"
#include "Game/Testcompy.h"
#include "Game/PositionDisplayComponent.h"

static void RegisterStaticComponents()
{
  // Static components
  RegisterStaticComponent<SpriteComponent>("SpriteComponent");
  RegisterStaticComponent<TransformComponent>("TransformComponent");
  RegisterStaticComponent<CameraComponent>("CameraComponent");
  RegisterStaticComponent<CustomModelComponent>("CustomModelComponent");
  RegisterStaticComponent<TextureComponent>("TextureComponent");
  RegisterStaticComponent<TestcompyComponent>("TestcompyComponent");
  RegisterStaticComponent<PositionDisplayComponent>("PositionDisplayComponent");
}

// ----------------------------------------------------------------------------
