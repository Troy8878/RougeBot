/*********************************
 * MapComponent.h
 * Claire Robsahm
 * Created 2014/09/19
 * Copyright � 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class MapComponentFactory;

// ----------------------------------------------------------------------------

// Class for other components to interface with the map.
class MapItem
{
public:
  MapItem() = default;

  // Shapes that can be drawn.
  enum Shapes
  {
    RECTANGLE,
    ELLIPSE
  };

  // Properties
  PROPERTY(get = _GetX, put = _SetX) int X;
  PROPERTY(get = _GetY, put = _SetY) int Y;
  PROPERTY(get = _GetColor, put = _SetColor) D2D1::ColorF Color;
  PROPERTY(get = _GetShape, put = _SetShape) Shapes Shape;
  PROPERTY(get = _GetGeometry) ID2D1Geometry *Geometry;
  PROPERTY(get = _GetBrush) ID2D1Brush *Brush;
  PROPERTY(get = _GetStairs, put = _SetStairs) bool Stairs;

  bool Visible = true;

  void Validate();
  void Release();
  void Draw(float mapScale, mrb_int mapSize);

private:
  int _x = 0, _y = 0; // Coordinates to draw at
  D2D1::ColorF _color = D2D1::ColorF(D2D1::ColorF::Black); // Color to draw
  Shapes _shape = Shapes::RECTANGLE; // Shape to use
  ID2D1Geometry *_geometry = nullptr; // The Direct2D shape
  ID2D1Brush *_brush = nullptr; // The brush to use

  bool _is_stairs = false;

  typedef GraphicsDevice::D2DData::clock clock;
  // Timestamp which says when we last updated.
  clock::time_point _timestamp;

public:
  // "Hidden" functions to get and set properties.
  int _GetX()
  {
    return _x;
  }

  void _SetX(int x)
  {
    _x = x;
    _timestamp = clock::from_time_t(0);
  }

  int _GetY()
  {
    return _y;
  }

  void _SetY(int y)
  {
    _y = y;
    _timestamp = clock::from_time_t(0);
  }

  D2D1::ColorF _GetColor()
  {
    return _color;
  }

  void _SetColor(const D2D1::ColorF &color)
  {
    _color = color;
    _timestamp = clock::from_time_t(0);
  }

  Shapes _GetShape()
  {
    return _shape;
  }

  void _SetShape(const Shapes &shape)
  {
    _shape = shape;
    _timestamp = clock::from_time_t(0);
  }

  ID2D1Geometry *_GetGeometry()
  {
    return _geometry;
  }

  ID2D1Brush *_GetBrush()
  {
    return _brush;
  }

  bool _GetStairs()
  {
    return _is_stairs;
  }

  void _SetStairs(bool value)
  {
    _is_stairs = value;
  }
};

// ----------------------------------------------------------------------------

class MapComponent : public Component
{
public:
  MapComponent();
  ~MapComponent();

  void Initialize(Entity *owner, const std::string &name) override;

  void OnUpdate(Events::EventMessage &);

  void OnMapUpdate(Events::EventMessage &);

  void DrawMap();

  // Map Item Functions
  MapItem *CreateMapItem();
  void DeleteMapItem(MapItem *item);

  mrb_value GetRubyWrapper() override;

  static MapComponentFactory factory;

  std::vector<std::vector<bool>> &GetExplored() { return _explored; }

private:
  Texture2D _texture;
  Entity *_floor = nullptr;
  mrb_value _floor_comp;
  mrb_value _map_obj;
  mrb_value _player_controller;

  // The player position
  int _px;
  int _py;

  // Track if we've found the stairs
  bool _stairs_found = false;

  // We need Vector to store where we've explored
  std::vector<std::vector<bool>> _explored;

  // We need another Vector for everything that wants to be drawn.
  std::vector<MapItem *> _items;
  std::vector<MapItem *> _revealed_items;

  bool _first_draw = true;

  void GetPlayerPosition(int &x, int &y);

  // Drawing resources
  struct DrawingResources
  {
    // typedef because dear god why
    typedef GraphicsDevice::D2DData::clock clock;
    typedef ID2D1Brush Brush;
    // Timestamp to keep track of when resources were made/changed
    clock::time_point timestamp;

    Brush *wallBrush = nullptr;
    Brush *playerBrush = nullptr;
    Brush *desatBrush = nullptr;
    Brush *unexploredBrush = nullptr;
    
    bool Validate();
    void Release();

    // Deconstructor
    ~DrawingResources()
    {
      Release();
    }
  } _drawing;
};

// ----------------------------------------------------------------------------

class MapComponentFactory final : public IComponentFactory
{
public:
  MapComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------