/*********************************
 * COMPONENTNAMEHEREComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
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

void COMPONENTNAMEHEREComponent::Initialize(Entity *owner, const std::string& name)
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
  void *memory, component_factory_data& data)
{
  (data); // do something with the serialization data

  auto *component = new (memory) COMPONENTNAMEHEREComponent;

  // do something to the component

  return component;
}

// ----------------------------------------------------------------------------

mrb_value COMPONENTNAMEHEREComponent::GetRubyWrapper()
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
