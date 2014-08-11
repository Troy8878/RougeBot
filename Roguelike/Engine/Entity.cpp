/*********************************
 * Entity.cpp
 * Jake Robsahm
 * Created 2014/08/11
 *********************************/

#include "Common.h"

bool Entity::canHandle(const Events::EventMessage& e)
{
  auto iterator = _events.find(e.eventId);
  return iterator != _events.end;
}

void handle(Events::EventMessage& e)
{

}

