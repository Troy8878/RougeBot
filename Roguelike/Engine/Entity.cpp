/*********************************
 * Entity.cpp
 * Jake Robsahm
 * Created 2014/08/11
 *********************************/

#include "Common.h"

// ----------------------------------------------------------------------------

Entity::Entity()
  : _id(CreateEntityId())
{
}

// ----------------------------------------------------------------------------

Component *Entity::AddComponent(const std::string& name, component_factory_data& data)
{
  auto *component = ComponentManager::Instance.InstantiateComponent(name, data);

  component->Initialize(this, name);
  _components[name] = component;

  return component;
}

// ----------------------------------------------------------------------------

bool Entity::CanHandle(const Events::EventMessage& e)
{
  auto iterator = _events.find(e.EventId);
  return iterator != _events.end() && !iterator->second.empty();
}

// ----------------------------------------------------------------------------

void Entity::Handle(Events::EventMessage& e)
{
  auto& handlers = _events[e.EventId];

  // Execute all of the handlers on the components
  for (auto& componentPair : handlers)
    // Member function pointer application is sooo weeiird D:
    (componentPair.first ->* componentPair.second)(e);
}

// ----------------------------------------------------------------------------

void Entity::AddEvent(Component *component, event_id id, component_handler handler)
{
  auto& handlers = _events[id];
  handlers[component] = handler;
}

// ----------------------------------------------------------------------------

void Entity::RemoveEvent(Component *component, event_id id)
{
  auto& handlers = _events[id];
  if (handlers.find(component) != handlers.end())
    handlers.remove(component);
}

// ----------------------------------------------------------------------------

entity_id Entity::CreateEntityId()
{
  THREAD_EXCLUSIVE_SCOPE;

  static entity_id id = 0;
  return ++id;
}

// ----------------------------------------------------------------------------

