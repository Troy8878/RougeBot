/*********************************
 * Entity.cpp
 * Jake Robsahm
 * Created 2014/08/11
 *********************************/

#include "Common.h"

#include "mruby/variable.h"
#include "mruby/string.h"

// ----------------------------------------------------------------------------

Entity::Entity()
  : _id(CreateEntityId())
{
}

// ----------------------------------------------------------------------------

Component *Entity::AddComponent(const std::string& name, component_factory_data& data)
{
  auto *component = ComponentManager::Instance.InstantiateComponent(name, data);

  component->Initialize(this, name);
  _components[name] = component;

  return component;
}

// ----------------------------------------------------------------------------

void Entity::RemoveComponent(const std::string& name)
{
  auto compit = _components.find(name);
  auto comp = compit->second;

  for (auto& event : _events)
  {
    event.second.erase(comp);
  }

  _components.erase(compit);
}

// ----------------------------------------------------------------------------

Component *Entity::GetComponent(const std::string& name)
{
  return _components.find(name)->second;
}

// ----------------------------------------------------------------------------

bool Entity::CanHandle(const Events::EventMessage& e)
{
  auto iterator = _events.find(e.EventId);
  return iterator != _events.end() && !iterator->second.empty();
}

// ----------------------------------------------------------------------------

void Entity::Handle(Events::EventMessage& e)
{
  auto& handlers = _events[e.EventId];

  // Execute all of the handlers on the components
  for (auto& componentPair : handlers)
    // Member function pointer application is sooo weeiird D:
    (componentPair.first ->* componentPair.second)(e);
}

// ----------------------------------------------------------------------------

void Entity::AddEvent(Component *component, event_id id, component_handler handler)
{
  auto& handlers = _events[id];
  handlers[component] = handler;
}

// ----------------------------------------------------------------------------

void Entity::RemoveEvent(Component *component, event_id id)
{
  auto& handlers = _events[id];
  if (handlers.find(component) != handlers.end())
    handlers.erase(component);
}

// ----------------------------------------------------------------------------

entity_id Entity::CreateEntityId()
{
  THREAD_EXCLUSIVE_SCOPE;

  static entity_id id = 0;
  return ++id;
}

// ----------------------------------------------------------------------------

ruby::ruby_value Entity::GetRubyWrapper()
{
  static auto rclass = GetWrapperRClass();

  if (rwrapper.is_nil())
  {
    auto mrb = ruby::ruby_engine::global_engine;
    auto rclass = GetWrapperRClass();
    rwrapper = rclass.new_inst(mrb->wrap_native_ptr(this));
    mrb->log_and_clear_error();
  }
  
  return rwrapper;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value ent_ptr_v;
  mrb_get_args(mrb, "o", &ent_ptr_v);

  static auto ent_ptr_sym = mrb_intern_cstr(mrb, "ent_ptr_v");
  mrb_iv_set(mrb, self, ent_ptr_sym, ent_ptr_v);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_get_component(mrb_state *mrb, mrb_value self)
{
  ruby::ruby_engine engine{mrb};
  
  mrb_value comp_name_v;
  mrb_get_args(mrb, "S", &comp_name_v);

  std::string comp_name = mrb_string_value_cstr(mrb, &comp_name_v);

  static auto ent_ptr_sym = mrb_intern_cstr(mrb, "ent_ptr_v");
  auto ent_ptr_v = mrb_iv_get(mrb, self, ent_ptr_sym);
  auto *entity = (Entity *) engine.unwrap_native_ptr(ent_ptr_v);

  auto comp = entity->GetComponent(comp_name);

  return comp->GetRubyWrapper().silent_reset();
}

// ----------------------------------------------------------------------------

ruby::ruby_class Entity::GetWrapperRClass()
{
  using namespace ruby;
  static ruby_class rclass;
  static bool init = false;

  if (init)
    return rclass;

  rclass = ruby_engine::global_engine->define_class("GameEntity");

  // TODO: Create class
  rclass.define_method("initialize",
                       rb_ent_initialize,
                       ARGS_REQ(1));
  rclass.define_method("get_component",
                       rb_ent_get_component,
                       ARGS_REQ(1));
  
  return rclass;
}

// ----------------------------------------------------------------------------

