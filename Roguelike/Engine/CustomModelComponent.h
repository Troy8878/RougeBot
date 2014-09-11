/*********************************
 * CustomModelComponent.h
 * Connor Hilarides
 * Created 2014/09/09
 *********************************/

#pragma once

#include "Common.h"
#include "Model.h"
#include "RenderSet.h"

#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class CustomModelComponentFactory;

// ----------------------------------------------------------------------------

class CustomModelComponent : public Component, public Drawable
{
public:
  CustomModelComponent(Shader *shader, RenderSet *set);
  ~CustomModelComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void Draw() override;
  void OnSetDestroyed() override { renderTarget = nullptr; }

  PROPERTY(get = _GetCustomModel, put = _SetCustomModel) Model *CustomModel;
  IR_PROPERTY(Shader *, ModelShader);

  static CustomModelComponentFactory factory;

private:
  Model *customModel = nullptr;
  RenderSet *renderTarget = nullptr;

public:
  inline Model *_GetCustomModel() const { return customModel; }
  void _SetCustomModel(Model *model);
};

// ----------------------------------------------------------------------------

class CustomModelComponentFactory : public IComponentFactory
{
public:
  CustomModelComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
