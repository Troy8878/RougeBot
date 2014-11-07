/*********************************
 * COMPONENTNAMEHEREComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "ComponentTemplate.h"

// ----------------------------------------------------------------------------

COMPONENTNAMEHEREComponentFactory COMPONENTNAMEHEREComponent::factory;

// ----------------------------------------------------------------------------

COMPONENTNAMEHEREComponent::COMPONENTNAMEHEREComponent()
{
}

// ----------------------------------------------------------------------------

void COMPONENTNAMEHEREComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);
}

// ----------------------------------------------------------------------------

COMPONENTNAMEHEREComponentFactory::COMPONENTNAMEHEREComponentFactory()
  : allocator(sizeof(COMPONENTNAMEHEREComponent))
{
}

// ----------------------------------------------------------------------------

Component *COMPONENTNAMEHEREComponentFactory::CreateObject(
  void *memory, component_factory_data &data)
{
  (data); // do something with the serialization data

  auto *component = new(memory) COMPONENTNAMEHEREComponent;

  // do something to the component

  return component;
}

// ----------------------------------------------------------------------------

mrb_value COMPONENTNAMEHEREComponent::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for COMPONENTNAMEHERE");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------