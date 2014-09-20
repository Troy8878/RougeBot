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

  mrb_value GetRubyWrapper() override;

  static MapComponentFactory factory;

private:
  Texture2D texture;
  Entity *floor = nullptr;
  
  // Drawing resources
  struct DrawingResources
  {
    // typedef because dear god why
    typedef GraphicsDevice::D2DData::clock clock;
    typedef ID2D1Brush Brush;
    // Timestamp to keep track of when resources were made/changed
    clock::time_point timestamp;

    Brush *lineBrush = nullptr;
    Brush *playerBrush = nullptr;
    
    void Validate();
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
