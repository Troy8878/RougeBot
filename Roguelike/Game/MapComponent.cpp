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

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void MapComponent::DrawingResources::Validate()
{
  using namespace D2D1;

  // Get a reference to direct2D
  auto& d2d = GetGame()->GameDevice->D2D;
  // Make sure the timestamp is okay
  if (timestamp >= d2d.ResourceTimestamp)
    return;
  // Release resources if it is invalid
  Release();

  HRESULT hr;

  ID2D1SolidColorBrush *scBrush;

  // Create the line brush
  hr = d2d.DeviceContext->CreateSolidColorBrush(ColorF(ColorF::White), &scBrush);
  CHECK_HRESULT(hr);
  lineBrush = scBrush;
  // Create the player brush
  hr = d2d.DeviceContext->CreateSolidColorBrush(ColorF(ColorF::Blue), &scBrush);
  CHECK_HRESULT(hr);
  playerBrush = scBrush;
}

// ----------------------------------------------------------------------------

void MapComponent::DrawingResources::Release()
{
  ReleaseDXInterface(lineBrush);
  ReleaseDXInterface(playerBrush);
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
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for MapComponent");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
