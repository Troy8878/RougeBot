/*********************************
 * RubyComponent.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers/BucketAllocator.h"

// ----------------------------------------------------------------------------

class RubyComponentFactory;

// ----------------------------------------------------------------------------

class RubyComponent : public Component
{
public:
  RubyComponent(ruby::ruby_class rclass, component_factory_data& data);

  void Initialize(Entity *owner, const std::string& name) override;
  void OnEvent(Events::EventMessage& e);

  void AddEventHandler(event_id event, mrb_sym handler);
  void RemoveEventHandler(event_id event);

  static RubyComponentFactory factory;

private:
  std::unordered_map<event_id, mrb_sym> events;
  mrb_value component_inst;

  friend class RubyComponentFactory;
};

// ----------------------------------------------------------------------------

class RubyComponentFactory : public IComponentFactory
{
public:
  RubyComponentFactory(ruby::ruby_class rclass);

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
  ruby::ruby_class rclass;
};

// ----------------------------------------------------------------------------


