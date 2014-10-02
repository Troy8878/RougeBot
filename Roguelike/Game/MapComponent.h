/*********************************
 * MapComponent.h
 * Jake Robsahm
 * Created 2014/09/19
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class MapComponentFactory;

// ----------------------------------------------------------------------------

class MapComponent : public Component
{
public:
  MapComponent();

  void Initialize(Entity *owner, const std::string& name) override;

  void OnUpdate(Events::EventMessage&);

  void OnMapUpdate(Events::EventMessage&);

  void DrawMap();

  mrb_value GetRubyWrapper() override;

  static MapComponentFactory factory;

private:
  Texture2D texture;
  Entity *floor = nullptr;
  mrb_value floor_comp;
  mrb_value player_controller;

  // We need Vector to store where we've explored
  std::vector<std::vector<bool>> explored;
  
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
    
    bool Validate();
    void Release();

    // Deconstructor
    ~DrawingResources() { Release(); }

  } drawing;

};

// ----------------------------------------------------------------------------

class MapComponentFactory : public IComponentFactory
{
public:
  MapComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------

// Class for other components to interface with the map.
class MapItem
{
public:
  // Shapes that can be drawn.
  enum Shapes{RECTANGLE, ELLIPSE};

  // Properties
  PROPERTY(get = _GetX, put = _SetX) int X;
  PROPERTY(get = _GetColor, put = _SetColor) D2D1::ColorF Color;
  PROPERTY(get = _GetShape, put = _SetShape) Shapes Shape;
  PROPERTY(get = _GetGeometry) ID2D1Geometry *Geometry;
  PROPERTY(get = _GetBrush) ID2D1Brush *Brush;


  void Validate();
  void Release();
  void Draw();

private:
  int _x, _y;                                                   // Coordinates to draw at
  D2D1::ColorF _color = D2D1::ColorF(D2D1::ColorF::Black);      // Color to draw
  Shapes _shape;                                                // Shape to use
  ID2D1Geometry *_geometry = 0;                                 // The Direct2D shape
  ID2D1Brush *_brush = 0;                                       // The brush to use

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

  D2D1::ColorF _GetColor()
  {
    return _color;
  }
  void _SetColor(const D2D1::ColorF& color)
  {
    _color = color;
    _timestamp = clock::from_time_t(0);
  }

  Shapes _GetShape()
  {
    return _shape;
  }
  void _SetShape(const Shapes& shape)
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
};

// ----------------------------------------------------------------------------


