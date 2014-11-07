/*********************************
 * Component.h
 * Connor Hilarides
 * Created 2014/08/11
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

struct ComponentRegistration;
__interface IComponentFactory;

// ----------------------------------------------------------------------------

class Component abstract
{
public:
  Component();

  // Please call this in your implementations
  // just at the top do Component::initialize(parent)
  virtual void Initialize(Entity *owner, const std::string &name);
  virtual void Cleanup();

  IR_PROPERTY(Entity *, Owner);
  IR_PROPERTY(std::string, Name);

  virtual mrb_value GetRubyWrapper()
  {
    return mrb_nil_value();
  }

  static std::vector<std::string> AdditionalDependencies()
  {
    return std::vector<std::string>{};
  }

protected:
  static ruby::ruby_module GetComponentRModule();
  static ruby::ruby_class GetComponentRClass();

  friend void RegisterEngineComponents();

  virtual ~Component()
  {
  }

  friend class ComponentManager;
};

// ----------------------------------------------------------------------------

class ComponentManager
{
public:
  typedef flat_map<std::string, ComponentRegistration> component_map;

  static ComponentManager Instance;

  void RegisterComponent(const ComponentRegistration &registration);
  PROPERTY(get = _GetComponentRegistrations) component_map ComponentRegistrations;

  Component *InstantiateComponent(const std::string &compName,
                                  component_factory_data &data);
  void ReleaseComponent(Component *component);

private:
  ComponentManager();

public:
  static component_map &_GetComponentRegistrations();
};

// ----------------------------------------------------------------------------

__interface IComponentFactory
{
  Component *CreateObject(void *memory, component_factory_data &data);
  IAllocator *Allocator();
};

// ----------------------------------------------------------------------------

struct ComponentRegistration
{
  ComponentRegistration(std::type_index const &componentType,
                        std::string const &componentName,
                        IComponentFactory *factory,
                        IAllocator *allocator)
    : componentName(componentName),
      componentType(componentType),
      factory(factory),
      allocator(allocator)
  {
  }

  std::string componentName;
  std::type_index componentType;
  IComponentFactory *factory;
  IAllocator *allocator;

  PROPERTY(get = _GetFactory) IComponentFactory &Factory;
  PROPERTY(get = _GetAllocator) IAllocator &Allocator;

private:
  ComponentRegistration()
    : componentType(typeid(int))
  {
  }

  friend class flat_map<std::string, ComponentRegistration>;

public:
  inline IComponentFactory &_GetFactory()
  {
    return *factory;
  }

  inline IAllocator &_GetAllocator()
  {
    return *allocator;
  }
};

// ----------------------------------------------------------------------------

void RegisterEngineComponents();

// ----------------------------------------------------------------------------

template <typename T>
void RegisterStaticComponent(const std::string &name,
                             IComponentFactory *factory = &T::factory)
{
  ComponentRegistration registration{typeid(T), name, factory, factory->Allocator()};
  ComponentManager::Instance.RegisterComponent(registration);

  auto prevfg = console::fg_color();
  std::cout << console::fg::green
    << "Registered static component '"
    << registration.componentName << "'" << std::endl
    << prevfg;
}

// ----------------------------------------------------------------------------

#define DRAWING_PROPERTY(type, name, var) \
  PROPERTY(get = _PropGet##name, put = _PropSet##name) type name; \
  type& _PropGet##name() { return var; } \
  void _PropSet##name(type const& value) { var = value; OnChanged(); }

// ----------------------------------------------------------------------------

inline void comp_add_property(mrb_state *mrb, RClass *cls, mrb_sym id,
                              mrb_sym type, bool can_set = false)
{
  static const mrb_sym add_property = mrb_intern_lit(mrb, "property");
  const mrb_value values[] =
    {
      mrb_symbol_value(id),
      mrb_symbol_value(type),
      mrb_bool_value(can_set)
    };
  mrb_funcall_argv(mrb, mrb_obj_value(cls), add_property, ARRAYSIZE(values), values);
}

inline void comp_add_property(mrb_state *mrb, RClass *cls, const char *id,
                              const char *type, bool can_set = false)
{
  mrb_sym id_sym = mrb_intern(mrb, id, strlen(id));
  mrb_sym type_sym = mrb_intern(mrb, type, strlen(type));
  comp_add_property(mrb, cls, id_sym, type_sym, can_set);
}

// ----------------------------------------------------------------------------