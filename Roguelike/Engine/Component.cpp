/*********************************
 * Component.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include "Common.h"

#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/value.h"
#include "mruby/variable.h"
#include "mruby/string.h"

// ----------------------------------------------------------------------------

ComponentManager ComponentManager::Instance;

// ----------------------------------------------------------------------------

void Component::Initialize(Entity *owner, const std::string& name)
{
  Owner = owner;
  Name = name;
}

// ----------------------------------------------------------------------------

void Component::Cleanup()
{
}

// ----------------------------------------------------------------------------

ComponentManager::ComponentManager()
{
}

// ----------------------------------------------------------------------------

void ComponentManager::RegisterComponent(const ComponentRegistration& registration)
{
  ComponentRegistrations[registration.componentName] = registration;
}

// ----------------------------------------------------------------------------

Component *ComponentManager::InstantiateComponent(const std::string& compName, 
                                                  component_factory_data& data)
{
  auto& regs = ComponentRegistrations;
  auto it = regs.find(compName);
  if (it == regs.end())
    throw string_exception("Component was requested to be created, "
                           "but is not a registered component (" + compName + ")");
  auto& reg = it->second;

  auto *memory = reg.Allocator.Allocate();
  auto *component = reg.Factory.CreateObject(memory, data);

  return component;
}

// ----------------------------------------------------------------------------

void ComponentManager::ReleaseComponent(Component *component)
{
  auto& reg = ComponentRegistrations[component->Name];
  
  component->~Component();
  reg.Allocator.Free(component);
}

// ----------------------------------------------------------------------------

flat_map<std::string, ComponentRegistration>& 
  ComponentManager::_GetComponentRegistrations()
{
  static flat_map<std::string, ComponentRegistration> registrations;
  return registrations;
}

// ----------------------------------------------------------------------------

ruby::ruby_module Component::GetComponentRModule()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  auto& engine = *ruby::ruby_engine::global_engine;

  if (init)
  {
    return engine.get_module("Components");
  }
  else
  {
    init = true;
    return engine.define_module("Components");
  }
}

// ----------------------------------------------------------------------------

#include "RubyComponent.h"

static mrb_value rb_component_register(mrb_state *_mrb, mrb_value self)
{
  auto& mrb = *ruby::ruby_engine::global_engine;
  assert(mrb == _mrb);
  auto comp_class = ruby::ruby_class{&mrb, (RClass *) mrb_ptr(self)};

  mrb_value comp_name_v;
  
  // register_component(comp_name_v:String)
  mrb_get_args(mrb, "S", &comp_name_v);

  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "component_name"), comp_name_v);

  auto factory = new RubyComponentFactory(comp_class);
  std::string comp_name = mrb_string_value_cstr(mrb, &comp_name_v);

  ComponentRegistration registration{typeid(RubyComponent), comp_name, 
                                     factory, factory->Allocator};
  ComponentManager::Instance.RegisterComponent(registration);

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
  auto& mrb = *ruby::ruby_engine::global_engine;
  assert(mrb == _mrb);

  mrb_value comp_class_v = mrb_obj_value(mrb_class(mrb, self));
  ruby::ruby_value comp_name;
  comp_name = mrb_iv_get(mrb, comp_class_v, 
                         mrb_intern_cstr(mrb, "component_name"));

  mrb_value data_hash;
  mrb_get_args(mrb, "H", &data_hash);

  static mrb_value 
    comp_ptr_key = mrb_symbol_value(mrb_intern_cstr(mrb, "comp_ptr_v"));

  mrb_value 
    comp_ptr_v = mrb_hash_get(mrb, data_hash, comp_ptr_key);

  mrb_iv_set(mrb, self, mrb_symbol(comp_ptr_key), comp_ptr_v);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_component_get_owner(mrb_state *_mrb, mrb_value self)
{
  auto& mrb = *ruby::ruby_engine::global_engine;
  assert(mrb == _mrb);
  
  static mrb_value comp_ptr_key = 
    mrb_symbol_value(mrb_intern_cstr(mrb, "comp_ptr_v"));
  mrb_value comp_ptr_v = mrb_iv_get(mrb, self, mrb_symbol(comp_ptr_key));
  
  Component *component = (Component *) mrb.unwrap_native_ptr(comp_ptr_v);

  return component->Owner->RubyWrapper;
}

// ----------------------------------------------------------------------------

ruby::ruby_class Component::GetComponentRClass()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  static ruby::ruby_class comp_class;

  if (init)
    return comp_class;
  
  auto& engine = *ruby::ruby_engine::global_engine;
  comp_class = engine.define_class("ComponentBase");

  comp_class.define_class_method("register_component",
                                 rb_component_register,
                                 ARGS_REQ(1));
  comp_class.define_method("initialize",
                           rb_component_initialize,
                           ARGS_REQ(1));
  comp_class.define_method("owner",
                           rb_component_get_owner,
                           ARGS_NONE());

  init = true;
  return comp_class;
}

// ----------------------------------------------------------------------------

#include "SpriteComponent.h"
#include "TransformComponent.h"

void RegisterEngineComponents()
{
  auto& rbengine = *ruby::ruby_engine::global_engine;
  Component::GetComponentRClass();

  RegisterStaticComponent<SpriteComponent>("SpriteComponent");
  RegisterStaticComponent<TransformComponent>("TransformComponent");

  rbengine.evaluate_asset("Components/SpinnyThingComponent.rb");
}

// ----------------------------------------------------------------------------
