/*********************************
 * Component.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

void Component::initialize(Entity& parent)
{
  _parent = &parent;
}

void Component::cleanup()
{
}

Entity& Component::getParent()
{
  return *_parent;
}

void ComponentManager::registerComponent(const ComponentRegistration& registration)
{
  auto& regs = components();
  regs[registration.componentName] = registration;
}

flat_map<std::string, ComponentRegistration>& ComponentManager::components()
{
  static flat_map<std::string, ComponentRegistration> registrations;
  return registrations;
}
