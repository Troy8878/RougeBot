/*********************************
 * Entity.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

class Component;

class Entity : public Events::EventReciever
{
public:
  typedef void(Component::*component_handler)(Events::EventMessage&);

  Entity();

  // Entities are going to have way too many references to move
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;

  /**
    Check if any of your components care about
    the eventId of this event message.
  */
  bool canHandle(const Events::EventMessage& e) override;
  /**
    Dispatch this event to all of your components
    that are waiting for it.
  */
  void handle(Events::EventMessage& e) override;
  
  /**
    This will be used when one of your components wants to
    connect to an event
  */
  void addEvent(Component *component, event_id id, void(Component::*)(Events::EventMessage&));
  /**
    This will be used when one of your components no longer wants
    to recieve an event
  */
  void removeEvent(Component *component, event_id id);

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
};

