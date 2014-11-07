/*********************************
 * PositionDisplayComponent.h
 * Connor Hilarides
 * Created 2014/09/16
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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
  PositionDisplayComponent(const std::string &entity, const std::wstring &font,
                           FLOAT fontSize);

  void Initialize(Entity *owner, const std::string &name) override;

  void OnFirstUpdate(Events::EventMessage &);
  void OnUpdate(Events::EventMessage &);

  void DrawDisplay();

  mrb_value GetRubyWrapper() override;

  static PositionDisplayComponentFactory factory;

private:
  Texture2D texture;
  TransformComponent *watchedTransform = nullptr;
  std::string entityName;
  math::Vector lastPos;

  // Drawing resources
  struct DrawingResources
  {
    typedef GraphicsDevice::D2DData::clock clock;

    clock::time_point timestamp;
    ID2D1SolidColorBrush *brush = nullptr;
    ID2D1SolidColorBrush *bgbrush = nullptr;
    IDWriteTextFormat *format = nullptr;

    std::wstring font;
    FLOAT fontSize;

    bool Validate();
    void Release();

    ~DrawingResources()
    {
      Release();
    }
  } drawing;
};

// ----------------------------------------------------------------------------

class PositionDisplayComponentFactory final : public IComponentFactory
{
public:
  PositionDisplayComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------