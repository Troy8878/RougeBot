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

#include "mruby/array.h"
#include "Engine/RubyWrappers.h"

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

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &MapComponent::OnUpdate);
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

    auto *player = GetGame()->CurrentLevel->RootEntity->FindEntity("Pancake");
    player_controller = player->GetComponent<RubyComponent>("PlayerControllerComponent")->GetRubyWrapper();
  }

  // This will only draw if it needs to be redrawn.
  if (drawing.Validate() || true)
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

  ConnorDraw();

  HRESULT hr = d2d.EndDraw();
  CHECK_HRESULT(hr);
}

// ----------------------------------------------------------------------------

void MapComponent::ConnorDraw()
{
  auto& d2d = GetGame()->GameDevice->D2D;

  d2d.DeviceContext->Clear();
  auto size = d2d.DeviceContext->GetSize();

  mrb_state *mrb = *mrb_inst;

  mrb_value ary = mrb_obj_iv_get(mrb, mrb_obj_ptr(floor_comp), mrb_intern_lit(*mrb_inst, "@room"));
  mrb_int len = mrb_ary_len(mrb, ary);

  auto mapScale = size.width / (len + 2);

  for (mrb_int y = 0; y < len; ++y)
  {
    mrb_value row = mrb_ary_entry(ary, y);
    mrb_int len = mrb_ary_len(mrb, row);

    for (mrb_int x = 0; x < len; ++x)
    {
      auto rect = D2D1::RectF((x + 1) * mapScale, (y + 1) * mapScale, 
                              (x + 2) * mapScale, (y + 2) * mapScale);
      mrb_int slot = mrb_fixnum(mrb_ary_entry(row, x));

      if (slot == 1) // Draw wall
      {
        d2d.DeviceContext->FillRectangle(rect, drawing.lineBrush);
      }
    }
  }

  // Borders
  {
    auto top = D2D1::RectF(0, 0, size.width, mapScale);
    auto left = D2D1::RectF(0, mapScale, mapScale, size.height - mapScale);
    auto right = D2D1::RectF(size.height - mapScale, mapScale, size.width, size.height - mapScale);
    auto bottom = D2D1::RectF(0, size.height - mapScale, size.width, size.height);

    d2d.DeviceContext->FillRectangle(top, drawing.lineBrush);
    d2d.DeviceContext->FillRectangle(left, drawing.lineBrush);
    d2d.DeviceContext->FillRectangle(right, drawing.lineBrush);
    d2d.DeviceContext->FillRectangle(bottom, drawing.lineBrush);
  }

  // Player Position
  {
    auto pos = ruby::get_ruby_vector(
      mrb_obj_iv_get(mrb, mrb_obj_ptr(player_controller), mrb_intern_lit(mrb, "@pos")));
    auto prect = D2D1::RectF((pos.x + 1) * mapScale, (len - pos.z + 1) * mapScale, 
                             (pos.x + 2) * mapScale, (len - pos.z) * mapScale);

    d2d.DeviceContext->FillRectangle(prect, drawing.playerBrush);
  }
}

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
  hr = d2d.DeviceContext->CreateSolidColorBrush(ColorF(ColorF::White, 0.8f), &scBrush);
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
