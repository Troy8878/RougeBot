/*********************************
 * Event.cpp
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#include "Common.h"
#include "Helpers\CriticalSections.h"

#include "mruby/class.h"

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
  ruby::ruby_gc_guard gc_guard(*mrb_inst);

  if (reciever.CanHandle(e))
    reciever.Handle(e);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_event_raise(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_events_init(mrb_state *mrb)
{
  auto *evclass = mrb_define_class(mrb, "Event", mrb->object_class);
  auto *enclass = mrb_define_class(mrb, "GameEntity", mrb->object_class);

  mrb_define_class_method(mrb, evclass, "raise_event", mrb_event_raise, ARGS_REQ(2));
  mrb_define_method(mrb, enclass, "raise_event", mrb_event_raise, ARGS_REQ(2));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_event_raise(mrb_state *mrb, mrb_value self)
{
  mrb_sym event_id;
  mrb_value event_data;
  mrb_get_args(mrb, "no", &event_id, &event_data);

  RubyEvent data_wrapper{event_data};
  EventMessage message{event_id, &data_wrapper, true};

  if (self.tt == MRB_TT_CLASS)
  {
    Event::Raise(message);
  }
  else
  {
    auto *entity = ruby::read_native_ptr<Entity>(mrb, self);
    Event::Raise(message, *entity);
  }

  return event_data;
}

// ----------------------------------------------------------------------------

