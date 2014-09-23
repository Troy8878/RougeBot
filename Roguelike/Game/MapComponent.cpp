/*********************************
 * MapComponent.cpp
 * Jake Robsahm
 * Created 2014/09/19
 *********************************/

#include "Common.h"
#include "MapComponent.h"
#include "Engine/RubyComponent.h"
#include "Engine/Texture.h"
#include "Engine/TextureComponent.h"
#include "Engine/Level.h"

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

void MapComponent::OnUpdate(Events::EventMessage&)
{
  // If we haven't found the floor yet, find it and store it.
  if (!floor)
  {
    floor = GetGame()->CurrentLevel->RootEntity->FindEntity("MainFloor");
    // Get the component that has the floor.
    floor_comp = floor->GetComponent<RubyComponent>("TestRoomComponent")->GetRubyWrapper();
  }
  // This will only draw if it needs to be redrawn.
  if (drawing.Validate())
  {
    DrawMap();
  }
}

// ----------------------------------------------------------------------------

void MapComponent::DrawMap()
{
  auto *texture_comp = Owner->GetComponent<TextureComponent>("TextureComponent");
  auto texture = texture_comp->Textures[0];

  auto& d2d = GetGame()->GameDevice->D2D;

  d2d.DrawTo(texture);

  auto size = d2d.DeviceContext->GetSize();

  d2d.DeviceContext->Clear();
  d2d.DeviceContext->DrawLine(
    D2D1::Point2F(0, 0), 
    D2D1::Point2F(size.width, size.height), 
    drawing.lineBrush, 5);

  HRESULT hr = d2d.EndDraw();
  CHECK_HRESULT(hr);
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

bool MapComponent::DrawingResources::Validate()
{
  using namespace D2D1;

  // Get a reference to direct2D
  auto& d2d = GetGame()->GameDevice->D2D;
  // Make sure the timestamp is okay
  if (timestamp >= d2d.ResourceTimestamp)
    return false;
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

  return true;
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
