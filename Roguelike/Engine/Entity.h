/*********************************
 * Entity.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

class Component;

typedef unsigned __int64 entity_id;

#ifndef COMP_FACTORY_DATA_DEF
#define COMP_FACTORY_DATA_DEF
typedef std::unordered_map<std::string, std::string> component_factory_data;
#endif

// ----------------------------------------------------------------------------

class Entity : public Events::EventReciever
{
public:
  typedef void(Component::*component_handler)(Events::EventMessage&);
  
  template <typename T>
  using derived_handler = void(T::*)(Events::EventMessage&);

  #pragma region Constructors and Properties

  Entity();

  // Entities are going to have way too many references to move
  NO_COPY_CONSTRUCTOR(Entity);
  NO_ASSIGNMENT_OPERATOR(Entity);

  PROPERTY(get = _GetEntityId) entity_id Id;

  #pragma endregion

  #pragma region Components

  /**
    Initialize a new component for this entity with the
    given name and component factory data
  */
  Component *AddComponent(const std::string& name, component_factory_data& data);
  
  /**
    Destruct the component with the given name from this
    entity
  */
  void RemoveComponent(const std::string& name);
  
  /**
    Returns the component on this entity with the given name
  */
  Component *GetComponent(const std::string& name);

  #pragma endregion

  #pragma region Events
  /**
    Check if any of your components care about
    the eventId of this event message.
  */
  bool CanHandle(const Events::EventMessage& e) override;
  /**
    Dispatch this event to all of your components
    that are waiting for it.
  */
  void Handle(Events::EventMessage& e) override;
  
  /**
    This will be used when one of your components wants to
    connect to an event
  */
  void AddEvent(Component *component, event_id id, component_handler handler);
  /**
    This will be used when one of your components no longer wants
    to recieve an event
  */
  void RemoveEvent(Component *component, event_id id);

  /**
    Helper for adding member functions of properly inheriting components
  */
  template <typename Derived>
  void AddEvent(Component *component, event_id id, derived_handler<Derived> handler)
  {
    AddEvent(component, id, static_cast<component_handler>(handler));
  }

  #pragma endregion

  #pragma region Protected fields
protected:
  /**
    Keep track of all your components :)
  */
  std::unordered_map<std::string, Component *> _components;

  /**
    Store all of your registered events here, keyed
    on the event id for fast lookup ;)
  */
  std::unordered_map<event_id, flat_map<Component *, component_handler>> _events;
  #pragma endregion

  #pragma region Other fields and helpers
public:
  entity_id _GetEntityId();

private:
  entity_id _id;

  static entity_id CreateEntityId();
  #pragma endregion

};

// ----------------------------------------------------------------------------

