/*********************************
 * Component.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

struct ComponentRegistration;
class ComponentFactory;

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

class ComponentFactory abstract
{
public:
  virtual Component *operator()(void *memory, component_factory_data& data) = 0;
  virtual Allocator *GetAllocator() = 0;

  PROPERTY(get = GetAllocator) Allocator *Allocator;

protected:
  virtual ~ComponentFactory()
  {
  }
};

// ----------------------------------------------------------------------------

struct ComponentRegistration
{
  ComponentRegistration(std::type_index const& componentType,
                        std::string const& componentName,
                        ComponentFactory *factory,
                        Allocator *allocator)
    : componentName(componentName),
      componentType(componentType),
      factory(factory),
      allocator(allocator)
  {
  }

  std::string componentName;
  std::type_index componentType;
  ComponentFactory *factory;
  Allocator *allocator;

  
  inline ComponentFactory& _GetFactory() { return *factory; }
  PROPERTY(get = _GetFactory) ComponentFactory& Factory;

  inline Allocator& _GetAllocator() { return *allocator; }
  PROPERTY(get = _GetAllocator) Allocator& Allocator;

private:
  ComponentRegistration()
    : componentType(typeid(int))
  {
  }

  friend class flat_map<std::string, ComponentRegistration>;
};

// ----------------------------------------------------------------------------

void RegisterEngineComponents();

// ----------------------------------------------------------------------------

template <typename T>
void RegisterStaticComponent(const std::string& name,
                             ComponentFactory *factory = &T::factory)
{
  ComponentRegistration registration{typeid(T), name, factory, factory->Allocator};
  ComponentManager::Instance.RegisterComponent(registration);
}

// ----------------------------------------------------------------------------

