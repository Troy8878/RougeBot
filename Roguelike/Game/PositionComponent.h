/*********************************
 * PositionComponent.h
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

// Just Ctrl+H "Position" with your component name

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class PositionComponentFactory;

// ----------------------------------------------------------------------------

class PositionComponent : public Component
{
public:
  enum class MovementMode
  {
    Jump,
    Linear,
    Exponential,
  };

  PositionComponent();

  void Initialize(Entity *owner, const std::string &name) override;
  void Update(Events::EventMessage &e);

  void Jump(float dt);
  void Linear(float dt);
  void Exponential(float dt);

  mrb_value GetRubyWrapper() override;

  static std::vector<std::string> AdditionalDependencies()
  {
    return std::vector<std::string>
    {
      "TransformComponent"
    };
  }

  static PositionComponentFactory factory;

  IRW_PROPERTY(float, MoveSpeed);
  PROPERTY(get = GetMode, put = SetMode) MovementMode Mode;
  PROPERTY(get = GetPos, put = SetPos) math::Vector Position;

private:
  MovementMode mode;
  void (PositionComponent::*modeFunc)(float dt);

  math::Vector position;
  math::Vector *translation;

public:
  MovementMode GetMode() { return mode; }
  void SetMode(MovementMode mode);

  math::Vector &GetPos() { return position; }
  const math::Vector &GetPos() const { return position; }
  void SetPos(const math::Vector &pos) { position = pos; }
};

// ----------------------------------------------------------------------------

class PositionComponentFactory final : public IComponentFactory
{
public:
  PositionComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
