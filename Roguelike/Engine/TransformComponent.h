/*********************************
 * TransformComponent.h
 * Jake Robsahm
 * Created 2014/08/22
 *********************************/

#pragma once

#include "Common.h"

#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class TransformComponentFactory;

// ----------------------------------------------------------------------------

class TransformComponent : public Component
{
public:
  TransformComponent(const math::Vector& position, const math::Vector& rotation, 
                     const math::Vector& scale);

  void Initialize(Entity *owner, const std::string& name) override;

  PROPERTY(get = _GetIsStatic, put = _SetIsStatic) bool Static;

  math::Vector Position;
  math::Vector Rotation;
  math::Vector Scale;

  void OnUpdate(Events::EventMessage&);
  void UpdateMatrix();
  void ApplyParentTransforms();

  mrb_value GetRubyWrapper() override;

  static TransformComponentFactory factory;

private:
  bool _static = false;

public:
  bool _GetIsStatic() { return _static; }
  void _SetIsStatic(bool value);
};

// ----------------------------------------------------------------------------

class TransformComponentFactory : public IComponentFactory
{
public:
  TransformComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;

  math::Vector ParseVector(json::value jv);
};

// ----------------------------------------------------------------------------

