/*********************************
 * Entity.cpp
 * Jake Robsahm
 * Created 2014/08/11
 *********************************/

#include "Common.h"

bool Entity::canHandle(const Events::EventMessage& e)
{
  auto iterator = _events.find(e.eventId());
  return iterator != _events.end() && !iterator->second.empty();
}

void Entity::handle(Events::EventMessage& e)
{
  auto& handlers = _events[e.eventId()];

  // Execute all of the handlers on the components
  for (auto& componentPair : handlers)
    // Member function pointer application is sooo weeiird D:
    (componentPair.first ->* componentPair.second)(e);
}

