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

event_id Event::CreateEventId(const std::string &name)
{
  THREAD_EXCLUSIVE_SCOPE;

  return mrb_intern_cstr(*mrb_inst, name.c_str());
}

// ----------------------------------------------------------------------------

static std::stack<EventMessage *> event_stack;

void Event::Raise(EventMessage &e, EventReciever &reciever)
{
  ruby::ruby_gc_guard gc_guard(*mrb_inst);

  event_stack.emplace(&e);

  if (reciever.CanHandle(e))
    reciever.Handle(e);

  event_stack.pop();
}

void Event::CustomRaise(EventMessage &e, std::function<void(EventMessage &)> raise)
{
  ruby::ruby_gc_guard gc_guard(*mrb_inst);

  event_stack.emplace(&e);

  raise(e);

  event_stack.pop();
}

EventMessage &Event::GetCurrentEvent()
{
  return *event_stack.top();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_event_raise(mrb_state *mrb, mrb_value self);
static mrb_value mrb_event_handled(mrb_state *mrb, mrb_value);
static mrb_value mrb_event_handled_set(mrb_state *mrb, mrb_value);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_events_init(mrb_state *mrb)
{
  auto *evclass = mrb_define_class(mrb, "Event", mrb->object_class);

  mrb_define_class_method(mrb, evclass, "raise_event", mrb_event_raise, ARGS_REQ(2));

  mrb_define_class_method(mrb, evclass, "handled?", mrb_event_handled, ARGS_NONE());
  mrb_define_class_method(mrb, evclass, "handled=", mrb_event_handled_set, ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_event_raise(mrb_state *mrb, mrb_value)
{
  mrb_sym event_id;
  mrb_value event_data = mrb_nil_value();
  mrb_get_args(mrb, "n|o", &event_id, &event_data);

  RubyEvent data_wrapper{event_data};
  EventMessage message{event_id, &data_wrapper, true};

  Event::Raise(message);

  return event_data;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_event_handled(mrb_state *, mrb_value)
{
  return mrb_bool_value(Event::GetCurrentEvent().Handled);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_event_handled_set(mrb_state *mrb, mrb_value)
{
  mrb_bool value;
  mrb_get_args(mrb, "b", &value);

  try
  {
    Event::GetCurrentEvent().Handled = !!value;
  }
  catch (std::exception &e)
  {
    mrb_raise(mrb, mrb->eException_class, e.what());
  }

  return mrb_bool_value(value);
}

// ----------------------------------------------------------------------------