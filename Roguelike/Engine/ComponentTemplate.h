/*********************************
 * COMPONENTNAMEHEREComponent.h
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

// Just Ctrl+H "COMPONENTNAMEHERE" with your component name

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class COMPONENTNAMEHEREComponentFactory;

// ----------------------------------------------------------------------------

class COMPONENTNAMEHEREComponent : public Component
{
public:
  COMPONENTNAMEHEREComponent();

  void Initialize(Entity *owner, const std::string& name) override;

  mrb_value GetRubyWrapper() override;

  static COMPONENTNAMEHEREComponentFactory factory;
};

// ----------------------------------------------------------------------------

class COMPONENTNAMEHEREComponentFactory : public IComponentFactory
{
public:
  COMPONENTNAMEHEREComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
