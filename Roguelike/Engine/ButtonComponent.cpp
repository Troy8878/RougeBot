/*********************************
 * ButtonComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

#include "Common.h"
#include "ButtonComponent.h"

// ----------------------------------------------------------------------------

ButtonComponentFactory ButtonComponent::factory;

// ----------------------------------------------------------------------------

ButtonComponent::ButtonComponent()
{
}

// ----------------------------------------------------------------------------

void ButtonComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  static Events::EventId updateId("update");
  Owner->AddEvent(this, updateId, &ButtonComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

ButtonComponentFactory::ButtonComponentFactory()
  : allocator(sizeof(ButtonComponent))
{
}

// ----------------------------------------------------------------------------

Component *ButtonComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  (data); // do something with the serialization data

  auto *component = new (memory) ButtonComponent;

  // do something to the component

  return component;
}

// ----------------------------------------------------------------------------

mrb_value ButtonComponent::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for Button");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
