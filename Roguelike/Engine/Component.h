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
typedef std::unordered_map<std::string, std::string> component_factory_data;

// ----------------------------------------------------------------------------

class Component abstract
{
public:
  virtual ~Component()
  {
  }

  // Please call this in your implementations
  // just at the top do Component::initialize(parent)
  virtual void Initialize(Entity *owner);
  virtual void Cleanup();

  IR_PROPERTY(Entity *, Owner);
};

// ----------------------------------------------------------------------------

class ComponentManager
{
public:
  typedef flat_map<std::string, ComponentRegistration> component_map;

  static ComponentManager Instance;

  void RegisterComponent(const ComponentRegistration& registration);
  __declspec(property(get = _GetComponents)) component_map Components;

private:
  ComponentManager();

public:
  static component_map& _GetComponents();
};

// ----------------------------------------------------------------------------

class ComponentFactory abstract
{
public:
  virtual Component* operator()(const component_factory_data& data) = 0;

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
                        ComponentFactory *factory)
    : componentName(componentName),
      componentType(componentType),
      factory(factory)
  {
  }

  std::string componentName;
  std::type_index componentType;
  ComponentFactory *factory;

private:
  ComponentRegistration()
    : componentType(typeid(int))
  {
  }

  friend class flat_map<std::string, ComponentRegistration>;
};

// ----------------------------------------------------------------------------

template <typename T>
class StaticComponentRegistration final
{
public:
  explicit StaticComponentRegistration(const std::string& name = typeid(T).name(),
                                       ComponentFactory* factory = &T::factory)
  {
    ComponentRegistration registration{typeid(T), typeid(T).name(), factory};
    ComponentManager::Instance.RegisterComponent(registration);
  }

  StaticComponentRegistration(const StaticComponentRegistration&) = delete;
  StaticComponentRegistration& operator=(const StaticComponentRegistration&) = delete;
};

// ----------------------------------------------------------------------------

