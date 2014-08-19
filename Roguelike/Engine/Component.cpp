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

void Component::Initialize(Entity *owner)
{
  Owner = owner;
}

// ----------------------------------------------------------------------------

void Component::Cleanup()
{
}

// ----------------------------------------------------------------------------

void ComponentManager::RegisterComponent(const ComponentRegistration& registration)
{
  Components[registration.componentName] = registration;
}

// ----------------------------------------------------------------------------

flat_map<std::string, ComponentRegistration>& ComponentManager::_GetComponents()
{
  static flat_map<std::string, ComponentRegistration> registrations;
  return registrations;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
