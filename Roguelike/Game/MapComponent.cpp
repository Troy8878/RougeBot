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

#pragma warning (disable : 4127) // This doesn't even need to be a warning really :/

// ----------------------------------------------------------------------------

MapComponentFactory MapComponent::factory;

// Ruby stuff for MapComponent
static void mrb_mapcomponent_init(mrb_state *mrb);
static mrb_value mrb_mapcomponent_new(mrb_state *mrb, MapComponent *map);
static void mrb_mapcomponent_free(mrb_state *, void *) {};

// Define the ruby functions for MapComponent
static mrb_value mrb_mapcomponent_create_item(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapcomponent_delete_item(mrb_state *mrb, mrb_value self);

// Stuff for creating component instances.
static RClass *cbase;
static mrb_data_type mrb_mapcomponent_data_type;

// Ruby stuff for MapItem
static mrb_value mrb_mapitem_new(mrb_state *mrb, MapItem *item);
static void mrb_mapitem_free(mrb_state *, void *) {};

// Define the ruby functions for MapItem
static mrb_value mrb_mapitem_getx(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_setx(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_gety(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_sety(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_getcolor(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_setcolor(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_getshape(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_setshape(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_getvisible(mrb_state *mrb, mrb_value self);
static mrb_value mrb_mapitem_setvisible(mrb_state *mrb, mrb_value self);

// Stuff for creating class instances.
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

  GetRubyWrapper();
}

// ----------------------------------------------------------------------------

void MapComponent::OnUpdate(Events::EventMessage&)
{
  // If we haven't found the floor yet, find it and store it.
  if (!_floor)
  {
    mrb_state *mrb = *mrb_inst;
    _floor = GetGame()->CurrentLevel->RootEntity->FindEntity("MainFloor");
    // Get the component that has the floor.
    _floor_comp = _floor->GetComponent<RubyComponent>("FloorGeneratorComponent")->GetRubyWrapper();
    _map_obj = mrb_funcall_argv(mrb, _floor_comp, mrb_intern_lit(mrb, "floor"), 0, nullptr);

    // Initialize explored vector.
    mrb_int rows = ruby::enumerable_length(mrb, _map_obj);
    _explored.resize((size_t) rows);

    mrb_value rubyRow = ruby::enumerable_first(mrb, _map_obj);
    mrb_int cols = ruby::enumerable_length(mrb, rubyRow);

    // Initialize each inividual row.
    for (auto& row : _explored)
    {
      row.resize((size_t) cols);
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
  _drawing.timestamp = DrawingResources::clock::from_time_t(0);
  //_drawing.Validate();
  //DrawMap();
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
  // Clear any previous transform
  d2d.DeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
  // Clear the currently existing pixels.
  d2d.DeviceContext->Clear();
  // Get the size of the context (in this case, texture) we're drawing too.
  auto size = d2d.DeviceContext->GetSize();

  // Create a reference to the Ruby Engine
  mrb_state *mrb = *mrb_inst;
  mrb_int len = ruby::enumerable_length(mrb, _map_obj);

  // How many pixels per block of the map. We make it a bit bigger to include the edges.
  FLOAT mapScale = size.width / (len + 2);

  // Loop over all of the rows.
  for (mrb_int y = 0; y < len; ++y)
  {
    // Retrieve the current row and get its length
    mrb_value row = ruby::enumerable_at(mrb, _map_obj, y);
    mrb_int row_len = ruby::enumerable_length(mrb, _map_obj);

    // Now we need to loop over every value in this row.
    for (mrb_int x = 0; x < row_len; ++x)
    {
      // Retrieve the current index value of the row array.
      static mrb_sym type_id = mrb_intern_lit(mrb, "type_id");

      mrb_value tile = ruby::enumerable_at(mrb, row, x);
      mrb_int val = mrb_fixnum(mrb_funcall_argv(mrb, tile, type_id, 0, nullptr));

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

  for(auto *item : _items)
    item->Draw(mapScale, len);

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
  RUN_ONCE(mrb_mapcomponent_init(*mrb_inst));
  return mrb_mapcomponent_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

void MapItem::Draw(float mapScale, mrb_int mapSize)
{
  // Make sure we even want to draw this.
  if(!Visible)
    return;

  Validate();

  auto& d2d = GetGame()->GameDevice->D2D;

  // Set the scale and translation.
  auto scale = D2D1::Matrix3x2F::Scale(mapScale, mapScale);
  auto translation = D2D1::Matrix3x2F::Translation((float) _x + 1, (float) mapSize - _y);
  d2d.DeviceContext->SetTransform(translation * scale);

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
  default:
    _geometry = nullptr;
  }

  if (_geometry == nullptr)
    throw basic_exception("Unknown geometry type for map item");

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

static void mrb_mapcomponent_init(mrb_state *mrb)
{
  // Initialize the MapComponent data type.
  mrb_mapcomponent_data_type.dfree = ruby::data_nop_delete;
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

  // Define the methods for MapItem
  mrb_define_method(mrb, iclass, "x", mrb_mapitem_getx, ARGS_NONE());
  mrb_define_method(mrb, iclass, "x=", mrb_mapitem_setx, ARGS_REQ(1));
  mrb_define_method(mrb, iclass, "y", mrb_mapitem_gety, ARGS_NONE());
  mrb_define_method(mrb, iclass, "y=", mrb_mapitem_sety, ARGS_REQ(1));
  mrb_define_method(mrb, iclass, "color", mrb_mapitem_getcolor, ARGS_NONE());
  mrb_define_method(mrb, iclass, "color=", mrb_mapitem_setcolor, ARGS_REQ(1));
  mrb_define_method(mrb, iclass, "shape", mrb_mapitem_getshape, ARGS_NONE());
  mrb_define_method(mrb, iclass, "shape=", mrb_mapitem_setshape, ARGS_REQ(1));
  mrb_define_method(mrb, iclass, "visible?", mrb_mapitem_getvisible, ARGS_NONE());
  mrb_define_method(mrb, iclass, "visible=", mrb_mapitem_setvisible, ARGS_REQ(1));

  // Constants
  mrb_define_const(mrb, iclass, "RECTANGLE", mrb_fixnum_value(MapItem::Shapes::RECTANGLE));
  mrb_define_const(mrb, iclass, "ELLIPSE", mrb_fixnum_value(MapItem::Shapes::ELLIPSE));
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

static mrb_value mrb_mapitem_getx(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  return mrb_fixnum_value(item->X);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_setx(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  mrb_int x;
  mrb_get_args(mrb, "i", &x);
  item->X = (int) x;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_gety(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  return mrb_fixnum_value(item->Y);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_sety(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  mrb_int y;
  mrb_get_args(mrb, "i", &y);
  item->Y = (int) y;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_getcolor(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  return ruby::create_new_vector(item->Color);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_setcolor(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);

  // Get the color arguments.
  mrb_value rcolor;
  mrb_get_args(mrb, "o", &rcolor);

  // Support creating colors from both vectors and strings.
  D2D1::ColorF color = D2D1::ColorF(0);
  if (mrb_string_p(rcolor))
  {
    color = StringToColor(mrb_str_to_stdstring(rcolor));
  }
  else
  {
    color = ruby::get_ruby_vector(rcolor);
  }

  item->Color = color;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_getshape(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);

  return mrb_fixnum_value((mrb_int) item->Shape);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_setshape(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  
  mrb_int shape;
  mrb_get_args(mrb, "i", &shape);

  item->Shape = (MapItem::Shapes)shape;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_getvisible(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  return mrb_bool_value(item->Visible);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_mapitem_setvisible(mrb_state *mrb, mrb_value self)
{
  auto *item = (MapItem *) mrb_data_get_ptr(mrb, self, &mrb_mapitem_data_type);
  mrb_bool visible;
  mrb_get_args(mrb, "b", &visible);
  item->Visible = !!visible;
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------


