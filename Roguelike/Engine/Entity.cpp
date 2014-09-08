/*********************************
 * Entity.cpp
 * Jake Robsahm, Connor Hilarides
 * Created 2014/08/11
 *********************************/

#include "Common.h"
#include "Helpers\BucketAllocator.h"

#include "mruby/variable.h"
#include "mruby/string.h"
#include "mruby/class.h"
#include "mruby/array.h"
#include "mruby/gems/regexp.h"

#include "json/json.h"

#include <sstream>

// ----------------------------------------------------------------------------

static entity_id next_ent_id = 0;

// ----------------------------------------------------------------------------

Entity::Entity(entity_id id)
  : _id(id != UNASSIGNED_ENTITY_ID ? id : CreateEntityId()),
    _Parent(nullptr)
{
  if (_id >= next_ent_id)
    next_ent_id = _id + 1;
}

// ----------------------------------------------------------------------------

Entity::~Entity()
{
  DestroyChildren();

  for (auto& pair : _components)
  {
    ComponentManager::Instance.ReleaseComponent(pair.second);
  }
}

// ----------------------------------------------------------------------------

Component *Entity::AddComponent(const std::string& name, 
                                component_factory_data& data)
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
  auto it = _components.find(name);
  return it != _components.end() ? it->second : nullptr;
}

// ----------------------------------------------------------------------------

bool Entity::CanHandle(const Events::EventMessage& e)
{
  return _eventCounts[e.EventId] > 0;
}

// ----------------------------------------------------------------------------

void Entity::Handle(Events::EventMessage& e)
{
  auto& handlers = _events[e.EventId];

  // Execute all of the handlers on the components
  for (auto& componentPair : handlers)
    // Member function pointer application is sooo weeiird D:
    (componentPair.first ->* componentPair.second)(e);

  for (auto child : children)
  {
    if (child->CanHandle(e))
      child->Handle(e);
  }
}

// ----------------------------------------------------------------------------

void Entity::AddEvent(Component *component, event_id id, 
                      component_handler handler)
{
  auto& handlers = _events[id];
  handlers[component] = handler;

  RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

void Entity::RemoveEvent(Component *component, event_id id)
{
  auto& handlers = _events[id];
  if (handlers.find(component) != handlers.end())
    handlers.erase(component);

  RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

void Entity::RecalculateEventCounts()
{
  _eventCounts.clear();

  for (auto& event : _events)
  {
    _eventCounts[event.first] += event.second.size();
  }

  for (auto child : children)
  {
    for (auto& event : child->_events)
    {
      _eventCounts[event.first] += event.second.size();
    }
  }

  if (Parent)
    Parent->RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

entity_id Entity::CreateEntityId()
{
  THREAD_EXCLUSIVE_SCOPE;
  return next_ent_id++;
}

// ----------------------------------------------------------------------------

void Entity::AddChild(Entity *entity)
{
  if (std::find(children.begin(), children.end(), entity) != children.end())
    return;

  if (entity->Parent)
  {
    entity->Parent->RemoveChild(entity);
  }

  entity->Parent = this;

  children.push_back(entity);

  entity->RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

void Entity::RemoveChild(Entity *entity)
{
  auto it = std::find(children.begin(), children.end(), entity);
  if (it == children.end())
    return;

  entity->Parent = nullptr;

  children.erase(it);

  RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

Entity *Entity::FindEntity(entity_id id)
{
  if (Id == id)
    return this;

  for (auto child : children)
  {
    auto potential = child->FindEntity(id);
    if (potential)
      return potential;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

Entity *Entity::FindEntity(const std::string& name)
{
  if (Name == name)
    return this;

  for (auto child : children)
  {
    auto potential = child->FindEntity(name);
    if (potential)
      return potential;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

void Entity::SearchEntities(std::vector<Entity *>& results,
                            const std::string& namePattern,
                            bool partialMatch)
{
  if (partialMatch)
  {
    if (Name.find(namePattern) != Name.npos)
    {
      results.push_back(this);
    }
  }
  else
  {
    if (Name == namePattern)
    {
      results.push_back(this);
    }
  }

  for (auto child : children)
  {
    child->SearchEntities(results, namePattern, partialMatch);
  }
}

// ----------------------------------------------------------------------------

void Entity::SearchEntities(std::vector<Entity *>& results, 
                            const std::regex& namePattern)
{
  if (std::regex_search(Name, namePattern))
    results.push_back(this);

  for (auto child : children)
  {
    child->SearchEntities(results, namePattern);
  }
}

// ----------------------------------------------------------------------------

void Entity::DestroyChildren()
{
  for (auto child : children)
  {
    EntityFactory::DestroyEntity(child);
  }
}

// ----------------------------------------------------------------------------

mrb_value Entity::GetRubyWrapper()
{
  auto mrb = ruby::ruby_engine::global_engine;
  auto rclass = GetWrapperRClass();
  auto wrapper = rclass.new_inst(mrb->wrap_native_ptr(this)).silent_reset();
  mrb->log_and_clear_error();
  
  return wrapper;
}

// ----------------------------------------------------------------------------

static Entity *rb_ent_get_internal(mrb_state *mrb, mrb_value self)
{
  ruby::ruby_engine engine{mrb};
  static auto ent_ptr_sym = mrb_intern_cstr(mrb, "ent_ptr_v");
  auto ent_ptr_v = mrb_iv_get(mrb, self, ent_ptr_sym);
  return (Entity *) engine.unwrap_native_ptr(ent_ptr_v);
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value ent_ptr_v;
  mrb_get_args(mrb, "o", &ent_ptr_v);

  ruby::save_native_ptr(mrb, self, mrb_cptr(ent_ptr_v));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_id(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::read_native_ptr<Entity>(mrb, self);
  return mrb_fixnum_value((mrb_int) entity->Id);
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_name(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::read_native_ptr<Entity>(mrb, self);
  return mrb_str_new_cstr(mrb, entity->Name.c_str());
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_get_component(mrb_state *mrb, mrb_value self)
{
  ruby::ruby_engine engine{mrb};
  
  mrb_value comp_name_v;
  mrb_get_args(mrb, "S", &comp_name_v);

  std::string comp_name = mrb_string_value_cstr(mrb, &comp_name_v);

  auto *entity = ruby::read_native_ptr<Entity>(mrb, self);
  auto *comp = entity->GetComponent(comp_name);
  return comp ? comp->GetRubyWrapper() : mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_find_entity(mrb_state *mrb, mrb_value self)
{
  mrb_value identifier;
  mrb_get_args(mrb, "o", &identifier);
  
  auto parent = ruby::read_native_ptr<Entity>(mrb, self);
  if (mrb_string_p(identifier))
  {
    auto entity = parent->FindEntity(mrb_str_to_stdstring(identifier));
    if (entity)
      return entity->RubyWrapper;
  }
  else if (mrb_fixnum_p(identifier))
  {
    auto entity = parent->FindEntity((entity_id) mrb_fixnum(identifier));
    if (entity)
      return entity->RubyWrapper;
  }
  else
  {
    auto typeerror = mrb_class_get(mrb, "TypeError");
    mrb_raisef(mrb, typeerror, "identifier parameter (%S) must be String or Fixnum", identifier);
  }

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_search_entities(mrb_state *mrb, mrb_value self)
{
  mrb_value pattern;
  mrb_bool partial = true;
  mrb_get_args(mrb, "o|b", &pattern, &partial);

  std::vector<Entity *> resvect;

  auto parent = ruby::read_native_ptr<Entity>(mrb, self);
  
  if (mrb_string_p(pattern))
  {
    parent->SearchEntities(resvect, mrb_str_to_stdstring(pattern), !!partial);
  }
  else
  {
    auto& regexp = mrb_regexp_cppregex(mrb, pattern);
    parent->SearchEntities(resvect, regexp);
  }

  auto results = mrb_ary_new(mrb);
  for (auto entity : resvect)
  {
    mrb_ary_push(mrb, results, entity->RubyWrapper);
  }
  return results;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_children(mrb_state *mrb, mrb_value self)
{
  auto parent = ruby::read_native_ptr<Entity>(mrb, self);
  auto results = mrb_ary_new(mrb);
  for (auto child : parent->Children)
  {
    mrb_ary_push(mrb, results, child->RubyWrapper);
  }
  return results;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_components(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::read_native_ptr<Entity>(mrb, self);
  auto results = mrb_hash_new(mrb);
  for (auto& component : entity->_components)
  {
    auto key = mrb_str_new_cstr(mrb, component.first.c_str());
    auto value = component.second->GetRubyWrapper();

    mrb_hash_set(mrb, results, key, value);
  }

  return results;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_inspect(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::read_native_ptr<Entity>(mrb, self);
  std::ostringstream inspection;

  // Begin
  inspection << "#<Entity:";

  // Entity address
  inspection << "0x" << std::hex << reinterpret_cast<LONG_PTR>(entity) << std::dec;

  // ID
  inspection << ", id=" << entity->Id;

  // Name
  inspection << ", name=" << entity->Name;

  // Components
  inspection << ", components=[";
  bool first = true;
  for (auto& component : entity->_components)
  {
    if (!first)
      inspection << ",";
    else
      first = false;

    inspection << component.first;
  }
  inspection << "]";

  // End
  inspection << ">";

  return mrb_str_new_cstr(mrb, inspection.str().c_str());
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
  rclass.define_method("initialize", rb_ent_initialize, ARGS_REQ(1));

  rclass.define_method("id", rb_ent_id, ARGS_NONE());
  rclass.define_method("name", rb_ent_name, ARGS_NONE());

  rclass.define_method("get_component", rb_ent_get_component, ARGS_REQ(1));
  rclass.define_method("find_entity", rb_ent_find_entity, ARGS_REQ(1));
  rclass.define_method("search_entities", rb_ent_search_entities, MRB_ARGS_ARG(1, 1));

  rclass.define_method("children", rb_ent_children, ARGS_NONE());
  rclass.define_method("components", rb_ent_components, ARGS_NONE());

  rclass.define_method("inspect", rb_ent_inspect, ARGS_NONE());
  
  return rclass;
}

// ----------------------------------------------------------------------------

static BucketAllocator entityAllocator{sizeof(Entity)};

// ----------------------------------------------------------------------------

Entity *EntityFactory::CreateEntity(const std::string& entdef, 
                                    const entity_factory_data& data,
                                    entity_id entid)
{
  // Read entdef
  entity_factory_data entdata;
  {
    auto tree = ParseJsonAsset("Entities", entdef + ".entitydef");
    if (!tree.is_object_of<json::value::object_t>())
      throw std::exception("Incorrect entity definition file");

    auto parts = tree.as_object_of<json::value::object_t>();
    for (auto& part : parts)
    {
      auto& datamap = entdata[part.first];

      for (auto& prop : part.second)
      {
        if (prop.second.is(json::json_type::jstring))
        {
          datamap[prop.first] = prop.second.as_string();
        }
        else
        {
          datamap[prop.first] = prop.second.serialize();
        }
      }
    }
  }

  // Merge entdef with data
  for (auto& part : data)
  {
    auto& datamap = entdata[part.first];

    for (auto& prop : part.second)
    {
      datamap[prop.first] = prop.second;
    }
  }

  auto entity = entityAllocator.Create<Entity>(entid);

  // Create all of the components
  for (auto& cpair : entdata)
  {
    entity->AddComponent(cpair.first, cpair.second);
  }

  return entity;
}

// ----------------------------------------------------------------------------

void EntityFactory::DestroyEntity(Entity *entity)
{
  {
    static Events::EventId recieverDestroyedId("event_reciever_destroyed");
    Events::EventRecieverDestroyedEvent data{entity};
    Events::EventMessage message{recieverDestroyedId, &data, false};
    Events::Event::Raise(message);
  }

  entityAllocator.Destroy(entity);
}

// ----------------------------------------------------------------------------

