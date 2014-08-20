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
  auto *component = reg.Factory(memory, data);

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

flat_map<std::string, ComponentRegistration>& ComponentManager::_GetComponentRegistrations()
{
  static flat_map<std::string, ComponentRegistration> registrations;
  return registrations;
}

// ----------------------------------------------------------------------------

#include "SpriteComponent.h"

void RegisterEngineComponents()
{
  RegisterStaticComponent<SpriteComponent>("SpriteComponent");
}

// ----------------------------------------------------------------------------
