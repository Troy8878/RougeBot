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

// ----------------------------------------------------------------------------

class Entity : public Events::EventReciever
{
public:
  typedef void(Component::*component_handler)(Events::EventMessage&);

  Entity();

  // Entities are going to have way too many references to move
  NO_COPY_CONSTRUCTOR(Entity);
  NO_ASSIGNMENT_OPERATOR(Entity);

  __declspec(property(get = _GetEntityId)) entity_id Id;

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

protected:
  /**
    Keep track of all your components :)
  */
  flat_map<std::type_index, Component *> _components;

  /**
    Store all of your registered events here, keyed
    on the event id for fast lookup ;)
  */
  flat_map<event_id, flat_map<Component *, component_handler>> _events;

  // Property getters and setters.
public:
  entity_id _GetEntityId();

private:
  entity_id _id;

  static entity_id CreateEntityId();
};

// ----------------------------------------------------------------------------

