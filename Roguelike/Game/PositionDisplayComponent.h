/*********************************
 * PositionDisplayComponent.h
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

// Just Ctrl+H "PositionDisplay" with your component name

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class PositionDisplayComponentFactory;
class TransformComponent;

// ----------------------------------------------------------------------------

class PositionDisplayComponent : public Component
{
public:
  PositionDisplayComponent(const std::string& entity);

  void Initialize(Entity *owner, const std::string& name) override;

  void OnFirstUpdate(Events::EventMessage&);
  void OnUpdate(Events::EventMessage&);

  void DrawDisplay();

  mrb_value GetRubyWrapper() override;

  static PositionDisplayComponentFactory factory;

private:
  Texture2D texture;
  TransformComponent *watchedTransform;
  std::string *entityName;
  math::Vector lastPos;

  // Drawing resources
  struct DrawingResources
  {
    typedef GraphicsDevice::D2DData::clock clock;

    clock::time_point timestamp;
    ID2D1SolidColorBrush *brush;
    IDWriteTextFormat *format;

    void Validate();
    void Release();

    ~DrawingResources() { Release(); }
  } drawing;
};

// ----------------------------------------------------------------------------

class PositionDisplayComponentFactory : public IComponentFactory
{
public:
  PositionDisplayComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
