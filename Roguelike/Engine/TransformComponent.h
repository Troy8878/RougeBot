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

  IRW_PROPERTY(math::Vector, Position);
  IRW_PROPERTY(math::Vector, Rotation);
  IRW_PROPERTY(math::Vector, Scale);

  IR_PROPERTY(math::Matrix, Matrix);

  void OnUpdate(Events::EventMessage&);
  void UpdateMatrix();

  ruby::ruby_value GetRubyWrapper() override;

  static TransformComponentFactory factory;
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

  math::Vector ParseVector(const std::string& str);
};

// ----------------------------------------------------------------------------

