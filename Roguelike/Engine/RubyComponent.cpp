/*********************************
 * RubyComponent.cpp
 * Connor Hilarides
 * Created 2014/08/28
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "RubyComponent.h"

#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/value.h"
#include "mruby/variable.h"
#include "mruby/string.h"

// ----------------------------------------------------------------------------

RubyComponent::RubyComponent(ruby::ruby_class rclass, component_factory_data &data)
{
  auto &mrb = *ruby::ruby_engine::global_engine;

  component_inst = mrb.hash_from_map(data).silent_reset();

  static auto rclass_sym =
    mrb_symbol_value(mrb_intern_cstr(mrb, "new_inst_rclass"));
  auto rclass_v = mrb_obj_value(rclass.mrb_handle());

  mrb_hash_set(mrb, component_inst, rclass_sym, rclass_v);
}

// ----------------------------------------------------------------------------

RubyComponent::~RubyComponent()
{
  GCUnlockObj(gclock);
}

// ----------------------------------------------------------------------------

void RubyComponent::Cleanup()
{
  mrb_funcall(*mrb_inst, component_inst, "finalize", 0);
}

// ----------------------------------------------------------------------------

void RubyComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  auto &mrb = *ruby::ruby_engine::global_engine;

  static mrb_value
    entity_ptr_key = mrb_symbol_value(mrb_intern_cstr(mrb, "entity_ptr_v")),
    comp_ptr_key = mrb_symbol_value(mrb_intern_cstr(mrb, "comp_ptr_v"));

  mrb_value
    entity_ptr_v = mrb.wrap_native_ptr(owner),
    comp_ptr_v = mrb.wrap_native_ptr(static_cast<Component *>(this));

  auto map = component_inst;

  mrb_hash_set(mrb, map, entity_ptr_key, entity_ptr_v);
  mrb_hash_set(mrb, map, comp_ptr_key, comp_ptr_v);

  static auto rclass_sym =
    mrb_symbol_value(mrb_intern_cstr(mrb, "new_inst_rclass"));
  auto rclass_p = reinterpret_cast<RClass *>(mrb_object(mrb_hash_get(mrb, map, rclass_sym)));
  ruby::ruby_class rclass{&mrb, rclass_p};

  component_inst = rclass.new_inst(map).silent_reset();
  gclock = GCLockObj(component_inst);

  mrb.log_and_clear_error();
}

// ----------------------------------------------------------------------------

RubyComponentFactory::RubyComponentFactory(ruby::ruby_class rclass)
  : allocator(sizeof(RubyComponent)), rclass(rclass)
{
}

// ----------------------------------------------------------------------------

Component *RubyComponentFactory::CreateObject(void *memory,
                                              component_factory_data &data)
{
  return new(memory) RubyComponent(rclass, data);
}

// ----------------------------------------------------------------------------

void RubyComponent::OnEvent(Events::EventMessage &e)
{
  auto &mrb = *ruby::ruby_engine::global_engine;

  mrb_value edata = mrb_nil_value();
  if (e.Data)
  {
    edata = e.Data->GetRubyWrapper();
  }

  mrb_funcall_argv(mrb, component_inst, events[e.EventId], 1, &edata);
  mrb.log_and_clear_error();
}

// ----------------------------------------------------------------------------

void RubyComponent::AddEventHandler(event_id event, mrb_sym handler)
{
  RemoveEventHandler(event);

  events[event] = handler;
  Owner->AddEvent(this, event, &RubyComponent::OnEvent);
}

// ----------------------------------------------------------------------------

void RubyComponent::RemoveEventHandler(event_id event)
{
  if (events.find(event) == events.end())
    return;

  events.erase(event);
  Owner->RemoveEvent(this, event);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Basic component stuff

static mrb_value rb_component_register(mrb_state *_mrb, mrb_value self)
{
  ruby::ruby_gc_guard gcguard{*mrb_inst};

  auto &mrb = *ruby::ruby_engine::global_engine;
  assert(mrb == _mrb);
  auto comp_class = ruby::ruby_class{&mrb, static_cast<RClass *>(mrb_ptr(self))};
  
  std::string comp_name = mrb_class_name(mrb, comp_class);
  mrb_value comp_name_v = mrb_str_new(mrb, comp_name.c_str(), comp_name.size());
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "component_name"), comp_name_v);

  auto factory = new RubyComponentFactory(comp_class);

  ComponentRegistration registration{typeid(RubyComponent), comp_name,
    factory, factory->Allocator()};
  ComponentManager::Instance.RegisterComponent(registration);

#pragma region Load additional dependencies

  auto cbase = mrb_obj_value(mrb_class_get(mrb, "ComponentBase"));
  auto flush_deps = mrb_intern_lit(mrb, "flush_dependencies");
  mrb_value deps = mrb_funcall_argv(mrb, cbase, flush_deps, 0, nullptr);
  mrb_int dep_len = mrb_ary_len(mrb, deps);

  auto &dependencies = GetComponentDependencies();
  auto &depList = dependencies[comp_name];

  for (mrb_int i = 0; i < dep_len; ++i)
  {
    mrb_value dep = mrb_ary_entry(deps, i);
    depList.push_back(mrb_str_to_stdstring(dep));
  }

  std::cout << "Dependencies for " << comp_name << ": " << depList << std::endl;

#pragma endregion

  auto prevfg = console::fg_color();
  std::cout << console::fg::green
    << "Registered ruby component '"
    << comp_name << "'" << std::endl
    << prevfg;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_component_initialize(mrb_state *_mrb, mrb_value self)
{
  ruby::ruby_gc_guard gcguard{*mrb_inst};

  auto &mrb = *mrb_inst;
  assert(mrb == _mrb);

  mrb_value data_hash;
  mrb_get_args(mrb, "H", &data_hash);

  static mrb_value comp_ptr_key = mrb_symbol_value(mrb_intern_cstr(mrb, "comp_ptr_v"));
  mrb_value comp_ptr_v = mrb_hash_get(mrb, data_hash, comp_ptr_key);

  save_native_ptr(mrb, self, mrb_cptr(comp_ptr_v));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static Component *rb_component_get_ptr(mrb_state *mrb, mrb_value self)
{
  Component *component;
  if (self.tt == MRB_TT_DATA)
  {
    component = static_cast<Component *>(static_cast<RData *>(self.value.p)->data);
  }
  else
  {
    component = ruby::read_native_ptr<Component>(mrb, self);
  }
  return component;
}

// ----------------------------------------------------------------------------

static mrb_value rb_component_get_owner(mrb_state *_mrb, mrb_value self)
{
  auto &mrb = *mrb_inst;
  assert(mrb == _mrb);

  Component *component = rb_component_get_ptr(mrb, self);

  return component->Owner->RubyWrapper;
}

// ----------------------------------------------------------------------------

static mrb_value rb_component_register_event(mrb_state *_mrb, mrb_value self)
{
  auto &mrb = *mrb_inst;
  assert(mrb == _mrb);

  RubyComponent *component = static_cast<RubyComponent *>(rb_component_get_ptr(mrb, self));

  mrb_sym event_sym, handler_sym;
  mrb_get_args(mrb, "nn", &event_sym, &handler_sym);

  component->AddEventHandler(event_sym, handler_sym);

  return mrb_symbol_value(handler_sym);
}

// ----------------------------------------------------------------------------

static mrb_value rb_component_remove_event(mrb_state *mrb, mrb_value self)
{
  RubyComponent *component = static_cast<RubyComponent *>(rb_component_get_ptr(mrb, self));

  mrb_sym event_sym;
  mrb_get_args(mrb, "n", &event_sym);

  component->RemoveEventHandler(event_sym);

  return mrb_symbol_value(event_sym);
}

// ----------------------------------------------------------------------------

ruby::ruby_class Component::GetComponentRClass()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  static ruby::ruby_class comp_class;

  if (init)
    return comp_class;

  auto &engine = *ruby::ruby_engine::global_engine;
  comp_class = engine.define_class("ComponentBase");

  comp_class.define_class_method("register_component",
                                 rb_component_register,
                                 ARGS_ANY());

  comp_class.define_method("initialize", rb_component_initialize, ARGS_REQ(1));
  comp_class.define_method("owner", rb_component_get_owner, ARGS_NONE());
  comp_class.define_method("register_event", rb_component_register_event, ARGS_REQ(2));
  comp_class.define_method("remove_event", rb_component_remove_event, ARGS_REQ(1));

  comp_class.define_method("finalize", mrb_nop, ARGS_ANY());

  init = true;
  return comp_class;
}

// ----------------------------------------------------------------------------

ruby::ruby_module Component::GetComponentRModule()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  auto &engine = *ruby::ruby_engine::global_engine;

  if (init)
  {
    return engine.get_module("Components");
  }
  
  init = true;
  return engine.define_module("Components");
}

// ----------------------------------------------------------------------------