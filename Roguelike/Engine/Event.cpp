/*********************************
 * Event.cpp
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#include "Common.h"
#include "Helpers\CriticalSections.h"

using namespace Events;

// ----------------------------------------------------------------------------

EventDispatcher *Event::GlobalDispatcher;

// ----------------------------------------------------------------------------

event_id Event::CreateEventId(const std::string& name)
{
  THREAD_EXCLUSIVE_SCOPE;

  return mrb_intern_cstr(*mrb_inst, name.c_str());
}

// ----------------------------------------------------------------------------

void Event::Raise(EventMessage& e, EventReciever& reciever)
{
  if (reciever.CanHandle(e))
    reciever.Handle(e);
}

// ----------------------------------------------------------------------------

