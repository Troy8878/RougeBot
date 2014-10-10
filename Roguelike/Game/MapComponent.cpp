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
static void mrb_mapcomponent_init(mrb_state *mrb);
static mrb_value mrb_mapcomponent_new(mrb_state *mrb, MapComponent *map);
static void mrb_mapcomponent_free(mrb_state *, void *) {};

static mrb_value mrb_mapcomponent_create_item(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapcomponent_delete_item(mrb_state *mrb, mrb_value self);

static RClass *cbase;
static mrb_data_type mrb_mapcomponent_data_type;

static mrb_value mrb_mapitem_new(mrb_state *mrb, MapItem *item);
static void mrb_mapitem_free(mrb_state *, void *) {};

static mrb_data_type mrb_mapitem_data_type;



// ----------------------------------------------------------------------------

MapComponent::MapComponent()
{
  cbase = Component::GetComponentRClass();
}

// ----------------------------------------------------------------------------

MapComponent::~MapComponent()
{
  // Delete every MapItem.
  for (auto *item : _items)
    delete item;
}

// ----------------------------------------------------------------------------

void MapComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &MapComponent::OnUpdate);

  // Register a map_update event, so we only draw when asked.
  DEF_EVENT_ID(map_update);
  Owner->AddEvent(this, map_update, &MapComponent::OnMapUpdate);
}

// ----------------------------------------------------------------------------

void MapComponent::OnUpdate(Events::EventMessage&)
{
  // If we haven't found the floor yet, find it and store it.
  if (!_floor)
  {
    _floor = GetGame()->CurrentLevel->RootEntity->FindEntity("MainFloor");
    // Get the component that has the floor.
    _floor_comp = _floor->GetComponent<RubyComponent>("TestRoomComponent")->GetRubyWrapper();

    auto *player = GetGame()->CurrentLevel->RootEntity->FindEntity("Pancake");
    _player_controller = player->GetComponent<RubyComponent>("PlayerControllerComponent")->GetRubyWrapper();

    // Initialize explored vector.
    mrb_state *mrb = *mrb_inst;
    mrb_value ary = mrb_obj_iv_get(mrb, mrb_obj_ptr(_floor_comp), mrb_intern_lit(*mrb_inst, "@room"));
    _explored.resize(mrb_ary_len(mrb, ary));
    mrb_value rubyRow = mrb_ary_entry(ary, 0);
    // Initialize each inividual row.
    for (auto& row : _explored)
    {
      row.resize(mrb_ary_len(mrb, rubyRow));
    }
  }

  // This will only draw if it needs to be redrawn (ie the window has been resized).
  if (_drawing.Validate())
  {
    DrawMap();
  }
}

// ----------------------------------------------------------------------------

void MapComponent::OnMapUpdate(Events::EventMessage&)
{
  _drawing.Validate();
  DrawMap();
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
  mrb_value ary = mrb_obj_iv_get(mrb, mrb_obj_ptr(_floor_comp), mrb_intern_lit(*mrb_inst, "@room"));
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
        d2d.DeviceContext->FillRectangle(rectangle, _drawing.wallBrush);
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
  d2d.DeviceContext->FillRectangle(leftRectangle, _drawing.wallBrush);
  d2d.DeviceContext->FillRectangle(rightRectangle, _drawing.wallBrush);
  d2d.DeviceContext->FillRectangle(topRectangle, _drawing.wallBrush);
  d2d.DeviceContext->FillRectangle(botRectangle, _drawing.wallBrush);

  // Now we get to draw the player.
  // Retrieve the position from the ruby class.
  auto playerPos = mrb_funcall(mrb, _player_controller, "pos", 0);
  auto& posv = ruby::get_ruby_vector(playerPos);
  // Create the Ellipse.
  auto playerEllipse = D2D1::Ellipse({(posv.x + 1.5f) * mapScale, (len - posv.z + 0.5f) * mapScale},
                                      mapScale / 2, mapScale / 2);
  // Now draw them~!
  d2d.DeviceContext->FillEllipse(playerEllipse, _drawing.playerBrush);

  for(auto *item : _items)
    item->Draw(mapScale);

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
  hr = d2d.DeviceContext->CreateSolidColorBrush(ColorF(ColorF::Yellow), &scBrush);
  CHECK_HRESULT(hr);
  playerBrush = scBrush;

  // Update the timestamp
  timestamp = clock::now();

  return true;
}

// ----------------------------------------------------------------------------

void MapComponent::DrawingResources::Release()
{
  ReleaseDXInterface(wallBrush);
  ReleaseDXInterface(playerBrush);
}

// ----------------------------------------------------------------------------

MapItem *MapComponent::CreateMapItem()
{
  MapItem *item = new MapItem;
  _items.push_back(item);
  return item;
}

// ----------------------------------------------------------------------------

void MapComponent::DeleteMapItem(MapItem *item)
{
  auto it = std::find(_items.begin(), _items.end(), item);
  _items.erase(it);
  delete item;
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

void MapItem::Draw(float mapScale)
{
  // Make sure we even want to draw this.
  if(!visible)
    return;

  auto& d2d = GetGame()->GameDevice->D2D;

  // Set the scale and translation.
  auto scale = D2D1::Matrix3x2F::Scale(D2D1::SizeF(mapScale, mapScale));
  auto translation = D2D1::Matrix3x2F::Translation(D2D1::SizeF((float)_x + 1, (float) _y + 1));
  d2d.DeviceContext->SetTransform(scale * translation);

  // Draw the thing
  d2d.DeviceContext->FillGeometry(_geometry, _brush);
}
// ----------------------------------------------------------------------------

void MapItem::Validate()
{
  using namespace D2D1;

  // Get a reference to direct2D
  auto& d2d = GetGame()->GameDevice->D2D;
  // Make sure the timestamp is okay
  if (_timestamp >= d2d.ResourceTimestamp)
    return;
  // Release resources if it is invalid
  Release();

  HRESULT hr;

  ID2D1SolidColorBrush *scBrush;
  ID2D1RectangleGeometry *rectangle;
  ID2D1EllipseGeometry *ellipse;

  // Create the brush
  hr = d2d.DeviceContext->CreateSolidColorBrush(_color, &scBrush);
  CHECK_HRESULT(hr);
  _brush = scBrush;

  // Create the shape
  switch(_shape)
  {
  case RECTANGLE:
    d2d.Factory->CreateRectangleGeometry(D2D1::RectF(0, 0, 1, 1), &rectangle);
    _geometry = rectangle;
    break;
  case ELLIPSE:
    d2d.Factory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(0.5, 0.5), 0.5, 0.5), &ellipse);
    _geometry = ellipse;
    break;
  }

  // Update the timestamp.
  _timestamp = clock::now();
}

// ----------------------------------------------------------------------------


void MapItem::Release()
{
  ReleaseDXInterface(_brush);
  ReleaseDXInterface(_geometry);
}

// ----------------------------------------------------------------------------

mrb_value MapItem::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for MapComponent");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static void mrb_mapcomponent_init(mrb_state *mrb)
{
  // Initialize the MapComponent data type.
  mrb_mapcomponent_data_type.dfree = mrb_mapcomponent_free;
  mrb_mapcomponent_data_type.struct_name = "MapComponent";

  // Initialize the MapItem data type.
  mrb_mapitem_data_type.dfree = mrb_mapitem_free;
  mrb_mapitem_data_type.struct_name = "MapItem";

  // Define the two classes.
  RClass *cclass = mrb_define_class(mrb, "MapComponent", cbase);
  RClass *iclass = mrb_define_class(mrb, "MapItem", mrb->object_class);

  // Define the methods for MapComponent.
  mrb_define_method(mrb, cclass, "create_item", mrb_mapcomponent_create_item, ARGS_NONE());
  mrb_define_method(mrb, cclass, "delete_item", mrb_mapcomponent_delete_item, ARGS_REQ(1));

  (iclass);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapcomponent_new(mrb_state *mrb, MapComponent *map)
{
  auto *cclass = mrb_class_get(mrb, "MapComponent");
  auto object = mrb_data_object_alloc(mrb, cclass, map, &mrb_mapcomponent_data_type);
  return mrb_obj_value(object);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapcomponent_create_item(mrb_state *mrb, mrb_value self)
{
  auto *map = (MapComponent *) mrb_data_get_ptr(mrb, self, &mrb_mapcomponent_data_type);
  auto *item = map->CreateMapItem();

  return mrb_mapitem_new(mrb, item);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapcomponent_delete_item(mrb_state *mrb, mrb_value self)
{
  auto *map = (MapComponent *) mrb_data_get_ptr(mrb, self, &mrb_mapcomponent_data_type);
  mrb_value item;
  mrb_get_args(mrb, "o", &item);
  map->DeleteMapItem((MapItem *) mrb_data_get_ptr(mrb, item, &mrb_mapitem_data_type));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_new(mrb_state *mrb, MapItem *item)
{
  auto *cclass = mrb_class_get(mrb, "MapItem");
  auto object = mrb_data_object_alloc(mrb, cclass, item, &mrb_mapitem_data_type);
  return mrb_obj_value(object);
}

// ----------------------------------------------------------------------------
