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

flat_map<std::string, ComponentRegistration>& ComponentManager::_GetComponentRegistrations()
{
  static flat_map<std::string, ComponentRegistration> registrations;
  return registrations;
}

// ----------------------------------------------------------------------------

#include "SpriteComponent.h"
#include "TransformComponent.h"

void RegisterEngineComponents()
{
  RegisterStaticComponent<SpriteComponent>("SpriteComponent");
  RegisterStaticComponent<TransformComponent>("TransformComponent");
}

// ----------------------------------------------------------------------------

ruby::ruby_module Component::GetComponentRModule()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  auto& engine = *ruby::ruby_engine::global_engine;

  if (init)
  {
    return engine.get_module("Components");
  }
  else
  {
    init = true;
    return engine.define_module("Components");
  }
}

// ----------------------------------------------------------------------------

static mrb_value rb_component_register(mrb_state *mrb, mrb_value self)
{
  (mrb, self);
  return mrb_nil_value();

  
}

// ----------------------------------------------------------------------------

ruby::ruby_class Component::GetComponentRClass()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  static ruby::ruby_class comp_class;

  if (init)
    return comp_class;
  
  auto& engine = *ruby::ruby_engine::global_engine;
  comp_class = engine.define_class("ComponentBase");

  comp_class.define_class_method("register_component",
                                 rb_component_register,
                                 ARGS_REQ(1));

  init = true;
  return comp_class;
}

// ----------------------------------------------------------------------------
