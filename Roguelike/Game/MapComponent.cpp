/*********************************
 * MapComponent.cpp
 * Jake Robsahm
 * Created 2014/09/19
 *********************************/

#include "Common.h"
#include "MapComponent.h"

// ----------------------------------------------------------------------------

MapComponentFactory MapComponent::factory;

// ----------------------------------------------------------------------------

MapComponent::MapComponent()
{
}

// ----------------------------------------------------------------------------

void MapComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);
}

// ----------------------------------------------------------------------------

MapComponentFactory::MapComponentFactory()
  : allocator(sizeof(MapComponent))
{
}

// ----------------------------------------------------------------------------

Component *MapComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  (data); // do something with the serialization data

  auto *component = new (memory) MapComponent;

  // do something to the component

  return component;
}

// ----------------------------------------------------------------------------

mrb_value MapComponent::GetRubyWrapper()
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
