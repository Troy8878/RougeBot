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
  // Get the texture component so we can do stuff with it
  auto *texture_comp = Owner->GetComponent<TextureComponent>("TextureComponent");
  // Get the actual texture itself
  auto texture = texture_comp->Textures[0];

  // Initialize all of the Direct2D stuff
  auto& d2d = GetGame()->GameDevice->D2D;
  // This basically says what we're drawing to - in this case, the texture
  d2d.DrawTo(texture);
  // Clear the currently existing pixels.
  d2d.DeviceContext->Clear();
  // Get the size of the context (in this case, texture) we're drawing too.
  auto size = d2d.DeviceContext->GetSize();

  // Create a reference to the Ruby Engine
  mrb_state *mrb = *mrb_inst;
  // Get the room values from the ruby code.
  mrb_value ary = mrb_obj_iv_get(mrb, mrb_obj_ptr(floor_comp), mrb_intern_lit(*mrb_inst, "@room"));
  mrb_int len = mrb_ary_len(mrb, ary);

  // How many pixels per block of the map. We make it a bit bigger to include the edges.
  FLOAT mapScale = size.width / (len + 2);

  // Loop over all of the rows.
  for (mrb_int y = 0; y < len; ++y)
  {
    // Retrieve the current row.
    mrb_value row = mrb_ary_entry(ary, y);
    // Find the length of that row.
    mrb_int row_len = mrb_ary_len(mrb, row);
    // Now we need to loop over every value in this row.
    for (mrb_int x = 0; x < row_len; ++x)
    {
      // Retrieve the current index value of the row array.
      mrb_int val = mrb_fixnum(mrb_ary_entry(row, x));
      // If the val is 1, it means there's a wall. Draw.
      if (val == 1)
      {
        // Create a rectangle using our coordinates.
        auto rectangle = D2D1::RectF((x + 1) * mapScale, (y + 1) * mapScale,
                                     (x + 2) * mapScale, (y + 2) * mapScale);
        // Now we actually draw the Rectangle.
        d2d.DeviceContext->FillRectangle(rectangle, drawing.wallBrush);
      }
    }
  }

  // Now we're going to draw the edge of the map. Create the rectangles first.
  auto leftRectangle = D2D1::RectF(0, 0, mapScale, size.height);
  auto rightRectangle = D2D1::RectF(size.width - mapScale, 0, size.width, size.height);
  auto topRectangle = D2D1::RectF(mapScale, 0, size.width - mapScale, mapScale);
  auto botRectangle = D2D1::RectF(mapScale, size.height - mapScale,
                                  size.width - mapScale, size.height);

  // Now we do the actual drawing.
  d2d.DeviceContext->FillRectangle(leftRectangle, drawing.wallBrush);
  d2d.DeviceContext->FillRectangle(rightRectangle, drawing.wallBrush);
  d2d.DeviceContext->FillRectangle(topRectangle, drawing.wallBrush);
  d2d.DeviceContext->FillRectangle(botRectangle, drawing.wallBrush);

  // Now we get to draw the player.
  // Retrieve the position from the ruby class.
  auto playerPos = mrb_funcall(mrb, player_controller, "pos", 0);
  auto& posv = ruby::get_ruby_vector(playerPos);
  // Create the rectangle.s
  auto playerRectangle = D2D1::RectF((posv.x + 1) * mapScale, (len - posv.z) * mapScale, 
                                     (posv.x + 2) * mapScale, (len - posv.z + 1) * mapScale);
  // Now draw them~!
  d2d.DeviceContext->FillRectangle(playerRectangle, drawing.playerBrush);


  HRESULT hr = d2d.EndDraw();
  CHECK_HRESULT(hr);
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

  // Create the wall brush
  hr = d2d.DeviceContext->CreateSolidColorBrush(ColorF(ColorF::CornflowerBlue, 0.8f), &scBrush);
  CHECK_HRESULT(hr);
  wallBrush = scBrush;
  // Create the player brush
  hr = d2d.DeviceContext->CreateSolidColorBrush(ColorF(ColorF::Green), &scBrush);
  CHECK_HRESULT(hr);
  playerBrush = scBrush;

  return true;
}

// ----------------------------------------------------------------------------

void MapComponent::DrawingResources::Release()
{
  ReleaseDXInterface(wallBrush);
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
