/*********************************
 * Component.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

struct ComponentRegistration;
__interface IComponentFactory;

#ifndef COMP_FACTORY_DATA_DEF
#define COMP_FACTORY_DATA_DEF
typedef std::unordered_map<std::string, std::string> component_factory_data;
#endif

// ----------------------------------------------------------------------------

class Component abstract
{
public:
  virtual ~Component()
  {
  }

  // Please call this in your implementations
  // just at the top do Component::initialize(parent)
  virtual void Initialize(Entity *owner, const std::string& name);
  virtual void Cleanup();

  IR_PROPERTY(Entity *, Owner);
  IR_PROPERTY(std::string, Name);

  virtual ruby::ruby_value GetRubyWrapper()
  {
    return ruby::ruby_value{};
  }

protected:
  ruby::ruby_module GetComponentRModule();
  ruby::ruby_class GetComponentRClass();
};

// ----------------------------------------------------------------------------

class ComponentManager
{
public:
  typedef flat_map<std::string, ComponentRegistration> component_map;

  static ComponentManager Instance;

  void RegisterComponent(const ComponentRegistration& registration);
  PROPERTY(get = _GetComponentRegistrations) component_map ComponentRegistrations;

  Component *InstantiateComponent(const std::string& compName, 
                                  component_factory_data& data);
  void ReleaseComponent(Component *component);

private:
  ComponentManager();

public:
  static component_map& _GetComponentRegistrations();
};

// ----------------------------------------------------------------------------

__interface IComponentFactory
{
  Component *CreateObject(void *memory, component_factory_data& data);

  PROPERTY(get = _GetAllocator) IAllocator *Allocator;
  IAllocator *_GetAllocator();
};

// ----------------------------------------------------------------------------

struct ComponentRegistration
{
  ComponentRegistration(std::type_index const& componentType,
                        std::string const& componentName,
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
  
  PROPERTY(get = _GetFactory) IComponentFactory& Factory;
  PROPERTY(get = _GetAllocator) IAllocator& Allocator;

private:
  ComponentRegistration()
    : componentType(typeid(int))
  {
  }

  friend class flat_map<std::string, ComponentRegistration>;

public:
  inline IComponentFactory& _GetFactory() { return *factory; }
  inline IAllocator& _GetAllocator() { return *allocator; }
};

// ----------------------------------------------------------------------------

void RegisterEngineComponents();

// ----------------------------------------------------------------------------

template <typename T>
void RegisterStaticComponent(const std::string& name,
                             IComponentFactory *factory = &T::factory)
{
  ComponentRegistration registration{typeid(T), name, factory, factory->Allocator};
  ComponentManager::Instance.RegisterComponent(registration);
}

// ----------------------------------------------------------------------------

