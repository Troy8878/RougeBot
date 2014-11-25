/*********************************
 * Entity.cpp
 * Jake Robsahm, Connor Hilarides, Leonardo Saikali
 * Created 2014/08/11
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Helpers/BucketAllocator.h"
#include "Level.h"
#include "Game/Voodoo.h"

#include "mruby/variable.h"
#include "mruby/string.h"
#include "mruby/class.h"
#include "mruby/array.h"
#include "mruby/gems/regexp.h"

#include "json/json.h"

// ----------------------------------------------------------------------------

static entity_id next_ent_id = 0;

// ----------------------------------------------------------------------------

static BucketAllocator sequenceAllocator = BucketAllocator(sizeof(ActionSequence));
static std::deque<Entity *> death_row;

// ----------------------------------------------------------------------------

Entity::Entity(entity_id id)
  : _id(id != UNASSIGNED_ENTITY_ID ? id : CreateEntityId()),
    _Parent(nullptr), _Transform(DirectX::XMMatrixIdentity()),
    SelfRef(std::make_shared<Entity *>(this))
{
  if (_id >= next_ent_id)
    next_ent_id = _id + 1;

  RegisterNamehash();

  LocalTransform = DirectX::XMMatrixIdentity();
}

// ----------------------------------------------------------------------------

Entity::~Entity()
{
  *SelfRef = nullptr;

  DestroyChildren();

  if (!_components.empty())
  {
    for (auto &pair : _components)
    {
      ComponentManager::Instance.ReleaseComponent(pair.second);
    }
  }

  if (!_actionSequences.empty())
  {
    for (auto &pair : _actionSequences)
    {
      sequenceAllocator.Destroy(pair.second);
    }
  }

  UnregisterNamehash();
}

// ----------------------------------------------------------------------------

void Entity::_SetName(const std::string &name)
{
  UnregisterNamehash();
  _name = name;
  RegisterNamehash();
}

// ----------------------------------------------------------------------------

Component *Entity::AddComponent(const std::string &name,
                                component_factory_data &data)
{
  if (_components[name])
    RemoveComponent(name);

  auto *component = ComponentManager::Instance.InstantiateComponent(name, data);

  component->Initialize(this, name);
  _components[name] = component;

  return component;
}

// ----------------------------------------------------------------------------

void Entity::RemoveComponent(const std::string &name)
{
  auto compit = _components.find(name);
  auto comp = compit->second;

  for (auto &event : _events)
  {
    event.second.erase(comp);
  }

  ComponentManager::Instance.ReleaseComponent(comp);

  _components.erase(compit);
}

// ----------------------------------------------------------------------------

Component *Entity::GetComponent(const std::string &name)
{
  if (_components.empty())
    return nullptr;

  auto it = _components.find(name);
  return it != _components.end() ? it->second : nullptr;
}

// ----------------------------------------------------------------------------

bool Entity::CanHandle(const Events::EventMessage &e)
{
  return _eventCounts[e.EventId] > 0;
}

// ----------------------------------------------------------------------------

void Entity::Handle(Events::EventMessage &e)
{
  RaiseEvent(e);
}

// ----------------------------------------------------------------------------

void Entity::HandleComponents(Events::EventMessage &e)
{
  struct
  {
    Component *comp;
    component_handler handle;
  } handle_list[127];
  size_t handle_count = 0;

  for (auto &handler : _events[e.EventId])
  {
    auto &item = handle_list[handle_count++];
    item.comp = handler.first;
    item.handle = handler.second;
  }

  for (auto &item : array_iterator(handle_list, handle_count))
  {
    (item.comp->*item.handle)(e);
  }
}

// ----------------------------------------------------------------------------

void Entity::HandleProxies(Events::EventMessage& e)
{
  auto it = proxies.find(e.EventId);
  if (it != proxies.end())
  {
    static Entity *processed[1024];
    size_t process_count = 0;

    auto &proxy = it->second.maps;

    if (proxies.size() > 1024)
      throw basic_exception("I can't process more than 1024 proxies on a single entity");

    proxies_invalidated = false;
    bool was_invalidated = false;

    for (auto it = proxy.begin(); it != proxy.end(); ++it)
    {
      if (was_invalidated)
      {
        auto begin = processed;
        auto end = processed + process_count;
        if (std::find(begin, end, it->first) != end)
        {
          continue;
        }
      }

      it->second(e);
      processed[process_count++] = it->first;

      if (proxies_invalidated)
      {
        proxies_invalidated = false;
        was_invalidated = true;

        if (proxy.size() == 0)
          break;

        it = proxy.begin();
      }
    }
  }
}

// ----------------------------------------------------------------------------

void Entity::LocalEvent(Events::EventMessage &e)
{
  DEF_EVENT_ID(send);
  if (e.EventId == send)
    HandleSend(e);

  HandleComponents(e);

  // Update Transform with parents
  DEF_EVENT_ID(draw);
  if (e.EventId == draw)
  {
    ApplyParentTransforms();
  }

  DEF_EVENT_ID(update);
  if (e.EventId == update)
  {
    auto &time = GetGame()->Time;
    OnUpdate(float(time.Dt));
  }

  HandleProxies(e);
}

// ----------------------------------------------------------------------------

void Entity::RaiseEvent(Events::EventMessage &e)
{
  LocalEvent(e);

  for (size_t i = 0; i < children.size(); ++i)
  {
    auto *child = children[i];

    if (child->CanHandle(e))
      child->RaiseEvent(e);
  }
}

// ----------------------------------------------------------------------------

void Entity::SinkEvent(Events::EventMessage &e)
{
  LocalEvent(e);

  if (Parent)
  {
    Parent->SinkEvent(e);
  }
}

// ----------------------------------------------------------------------------

void Entity::AddEvent(Component *component, event_id id,
                      component_handler handler)
{
  auto &handlers = _events[id];
  handlers[component] = handler;

  RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

void Entity::RemoveEvent(Component *component, event_id id)
{
  auto &handlers = _events[id];
  if (handlers.find(component) != handlers.end())
    handlers.erase(component);

  RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

void Entity::AddProxy(Entity *entity, event_id id, EventProxyList::Func func)
{
  proxies[id].maps[entity] = func;
}

// ----------------------------------------------------------------------------

void Entity::RemoveProxy(Entity *entity, event_id id)
{
  auto &proxy = proxies[id];
  proxy.maps.erase(entity);

  if (proxy.maps.empty())
  {
    proxies.erase(id);
  }
}

// ----------------------------------------------------------------------------

void Entity::HandleSend(Events::EventMessage& e)
{
  mrb_state *mrb = *mrb_inst;
  mrb_value event_params = e.GetData<Events::RubyEvent>()->GetRubyWrapper();
  if (ruby::enumerable_length(mrb, event_params) != 2)
    return;

  mrb_sym sym = mrb_symbol(ruby::enumerable_at(mrb, event_params, 0));
  mrb_value aryval = ruby::enumerable_at(mrb, event_params, 1);
  RArray *ary = mrb_ary_ptr(aryval);
  
  for (auto &pair : Components)
  {
    auto comp = pair.second->GetRubyWrapper();
    if (mrb_respond_to(mrb, comp, sym))
    {
      mrb_funcall_argv(mrb, comp, sym, mrb_ary_len(mrb, aryval), ary->ptr);
    }
  }
}

// ----------------------------------------------------------------------------

void Entity::RecalculateEventCounts()
{
  DEF_EVENT_ID(update);
  DEF_EVENT_ID(draw);
  DEF_EVENT_ID(send);

  _eventCounts.clear();

  // Every entity handles these events for themselves,
  // regardless of what components they have.
  _eventCounts[update] = 1;
  _eventCounts[draw] = 1;
  _eventCounts[send] = 1;

  // Tally up the component handlers
  for (auto &event : _events)
  {
    _eventCounts[event.first] += event.second.size();
  }

  // Tally up the proxies
  for (auto &proxy : proxies)
  {
    _eventCounts[proxy.first] += proxy.second.maps.size();
  }

  // Tally the children's results
  for (auto child : children)
  {
    for (auto &count : child->_eventCounts)
    {
      _eventCounts[count.first] += count.second;
    }
  }

  // Have the parent do the same
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
  if (find(children.begin(), children.end(), entity) != children.end())
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
  auto it = find(children.begin(), children.end(), entity);
  if (it == children.end())
    return;

  entity->Parent = nullptr;

  children.erase(it);

  RecalculateEventCounts();
}

// ----------------------------------------------------------------------------

static std::unordered_map<entity_id, Entity *> idmap;
static std::unordered_multimap<std::string, Entity *> namemap;

// ----------------------------------------------------------------------------

void Entity::RegisterNamehash()
{
  idmap[_id] = this;
  namemap.insert({_name, this});
}

// ----------------------------------------------------------------------------

void Entity::UnregisterNamehash()
{
  typedef decltype(namemap) nmmt;

  idmap.erase(_id);

  auto namebucket = namemap.bucket(_name);
  namemap.erase(std::find(
    namemap.begin(namebucket), namemap.end(namebucket),
    nmmt::value_type{_name, this}));
}

// ----------------------------------------------------------------------------

Entity *Entity::FindEntity(entity_id id)
{
  auto it = idmap.find(id);
  return it == idmap.end() ? nullptr : it->second;
}

// ----------------------------------------------------------------------------

Entity *Entity::FindEntity(const std::string &name)
{
  auto bucket = namemap.bucket(name);
  auto first = namemap.begin(bucket);
  auto last = namemap.end(bucket);

  for (; first != last; ++first)
  {
    if (first->second->IsSelfOrChildOf(this))
      return first->second;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

Entity *Entity::LocalFind(const std::string &name)
{
  if (name == _name)
    return this;

  if (children.empty())
    return nullptr;

  for (auto &child : children)
  {
    auto res = child->LocalFind(name);
    if (res)
      return res;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

void Entity::SearchEntities(std::vector<Entity *> &results,
                            const std::string &namePattern,
                            bool partialMatch)
{
  if (!partialMatch)
  {
    auto bucket = namemap.bucket(namePattern);
    auto first = idmap.begin(bucket);
    auto last = idmap.end(bucket);

    for (; first != last; ++first)
    {
      if (first->second->IsSelfOrChildOf(this))
        results.push_back(first->second);
    }
  }
  else
  {
    for (size_t i = 0; i < namemap.bucket_count(); ++i)
    {
      auto iter = namemap.begin(i);
      auto end = namemap.end(i);

      if (iter == end)
        continue;

      // Check if this bucket matches the pattern
      if (iter->first.find(namePattern) == iter->first.npos)
        continue;

      for (; iter != end; ++iter)
      {
        if (iter->second->IsSelfOrChildOf(this))
          results.push_back(iter->second);
      }
    }
  }
}

// ----------------------------------------------------------------------------

void Entity::SearchEntities(std::vector<Entity *> &results,
                            const std::regex &namePattern)
{
  for (size_t i = 0; i < namemap.bucket_count(); ++i)
  {
    auto iter = namemap.begin(i);
    auto end = namemap.end(i);

    if (iter == end)
      continue;

    // Check if this bucket matches the pattern
    if (!regex_search(iter->first, namePattern))
      continue;

    for (; iter != end; ++iter)
    {
      if (iter->second->IsSelfOrChildOf(this))
        results.push_back(iter->second);
    }
  }
}

// ----------------------------------------------------------------------------

void Entity::DestroyChildren()
{
  if (children.empty())
    return;

  for (auto child : children)
  {
    EntityFactory::DestroyEntity(child);
  }
}

// ----------------------------------------------------------------------------

mrb_data_type rb_ent_dt;

// ----------------------------------------------------------------------------

mrb_value Entity::GetRubyWrapper()
{
  mrb_state *mrb = *mrb_inst;

  auto rclass = GetWrapperRClass();
  auto data = mrb_data_object_alloc(mrb, rclass, this, &rb_ent_dt);

  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_id(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::data_get<Entity>(mrb, self);
  return mrb_fixnum_value(static_cast<mrb_int>(entity->Id));
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_name(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::data_get<Entity>(mrb, self);
  return mrb_str_new_cstr(mrb, entity->Name.c_str());
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_get_component(mrb_state *mrb, mrb_value self)
{
  mrb_value comp_name_v;
  mrb_get_args(mrb, "S", &comp_name_v);

  std::string comp_name = mrb_string_value_cstr(mrb, &comp_name_v);

  auto *entity = ruby::data_get<Entity>(mrb, self);
  auto *comp = entity->GetComponent(comp_name);
  return comp ? comp->GetRubyWrapper() : mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_add_component(mrb_state *mrb, mrb_value self)
{
  component_factory_data data;

  const char *name;
  mrb_value hash;
  mrb_get_args(mrb, "zH", &name, &hash);

  json::value jdata;
  try
  {
    jdata = mrb_inst->hash_to_json(hash);
  }
  catch (std::exception &e)
  {
    mrb_raise(mrb, mrb_class_get(mrb, "RuntimeError"), e.what());
  }

  for (auto &item : jdata.as_object())
  {
    data[item.first] = item.second;
  }

  auto *entity = ruby::data_get<Entity>(mrb, self);

  try
  {
    auto *comp = entity->AddComponent(name, data);
    return comp->GetRubyWrapper();
  }
  catch (std::exception &e)
  {
    mrb_raise(mrb, mrb->eException_class, e.what());
  }
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_remove_component(mrb_state *mrb, mrb_value self)
{
  const char *name;
  mrb_get_args(mrb, "z", &name);

  auto *entity = ruby::data_get<Entity>(mrb, self);
  entity->RemoveComponent(name);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_proxy_event(mrb_state *mrb, mrb_value self)
{
  mrb_sym event;
  mrb_value target;
  mrb_sym target_sym;
  mrb_get_args(mrb, "non", &event, &target, &target_sym);

  Entity *entity = ruby::data_get<Entity>(mrb, self);
  mrb_value targetOwner = mrb_funcall(mrb, target, "owner", 0);
  Entity *targetEnt = ruby::data_get<Entity>(mrb, targetOwner);

  entity->AddProxy(targetEnt, event,
                   [mrb, target, target_sym](Events::EventMessage &e)
                   {
                     mrb_value edata = mrb_nil_value();
                     if (e.Data)
                     {
                       edata = e.Data->GetRubyWrapper();
                     }

                     mrb_funcall_argv(mrb, target, target_sym, 1, &edata);
                     mrb_inst->log_and_clear_error();
                   });

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_remove_proxy(mrb_state *mrb, mrb_value self)
{
  mrb_sym event;
  mrb_value target;
  mrb_get_args(mrb, "no", &event, &target);

  Entity *entity = ruby::data_get<Entity>(mrb, self);
  mrb_value targetOwner = mrb_funcall(mrb, target, "owner", 0);
  Entity *targetEnt = ruby::data_get<Entity>(mrb, targetOwner);

  entity->RemoveProxy(targetEnt, event);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_find_entity(mrb_state *mrb, mrb_value self)
{
  mrb_value identifier;
  mrb_get_args(mrb, "o", &identifier);

  auto parent = ruby::data_get<Entity>(mrb, self);
  if (mrb_string_p(identifier))
  {
    auto entity = parent->FindEntity(mrb_str_to_stdstring(identifier));
    if (entity)
      return entity->RubyWrapper;
  }
  else if (mrb_fixnum_p(identifier))
  {
    auto entity = parent->FindEntity(static_cast<entity_id>(mrb_fixnum(identifier)));
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

static mrb_value rb_ent_find_local(mrb_state *mrb, mrb_value self)
{
  mrb_value identifier;
  mrb_get_args(mrb, "S", &identifier);

  auto id = mrb_str_to_stdstring(identifier);
  auto entity = ruby::data_get<Entity>(mrb, self);

  auto result = entity->LocalFind(id);
  if (result)
    return result->RubyWrapper;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_search_entities(mrb_state *mrb, mrb_value self)
{
  mrb_value pattern;
  mrb_bool partial = true;
  mrb_get_args(mrb, "o|b", &pattern, &partial);

  std::vector<Entity *> resvect;

  auto parent = ruby::data_get<Entity>(mrb, self);

  if (mrb_string_p(pattern))
  {
    parent->SearchEntities(resvect, mrb_str_to_stdstring(pattern), !!partial);
  }
  else
  {
    auto &regexp = mrb_regexp_cppregex(mrb, pattern);
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
  auto parent = ruby::data_get<Entity>(mrb, self);
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
  auto entity = ruby::data_get<Entity>(mrb, self);
  auto results = mrb_hash_new(mrb);
  for (auto &component : entity->_components)
  {
    if (component.second == nullptr)
      continue;

    auto key = mrb_str_new_cstr(mrb, component.first.c_str());
    auto value = component.second->GetRubyWrapper();

    mrb_hash_set(mrb, results, key, value);
  }

  return results;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_inspect(mrb_state *mrb, mrb_value self)
{
  auto entity = ruby::data_get<Entity>(mrb, self);
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
  for (auto &component : entity->_components)
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

static mrb_value rb_ent_add_child(mrb_state *mrb, mrb_value self)
{
  mrb_value child_v;
  mrb_get_args(mrb, "o", &child_v);

  if (mrb_obj_class(mrb, child_v) != mrb_class_get(mrb, "GameEntity"))
    mrb_raise(mrb, mrb->eException_class, "Expected param to be GameEntity");

  auto *parent = ruby::data_get<Entity>(mrb, self);
  auto *child = ruby::data_get<Entity>(mrb, child_v);

  parent->AddChild(child);

  return child_v;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_remove_child(mrb_state *mrb, mrb_value self)
{
  mrb_value child_v;
  mrb_get_args(mrb, "o", &child_v);

  if (mrb_obj_class(mrb, child_v) != mrb_class_get(mrb, "GameEntity"))
    mrb_raise(mrb, mrb->eException_class, "Expected param to be GameEntity");

  auto *parent = ruby::data_get<Entity>(mrb, self);
  auto *child = ruby::data_get<Entity>(mrb, child_v);

  parent->RemoveChild(child);

  return child_v;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_create(mrb_state *mrb, mrb_value)
{
  ruby::ruby_gc_guard gcguard{mrb};

  MRB_DECL_SYM(mrb, id_sym, "id");
  MRB_DECL_SYM(mrb, name_sym, "name");
  MRB_DECL_SYM(mrb, arch_sym, "archetype");
  MRB_DECL_SYM(mrb, comp_sym, "components");

  mrb_value opts = mrb_nil_value();
  mrb_get_args(mrb, "|H", &opts);

  if (mrb_nil_p(opts))
    opts = mrb_hash_new(mrb);

  mrb_value mrb_id = mrb_hash_get(mrb, opts, id_sym_v);
  entity_id id = UNASSIGNED_ENTITY_ID;
  if (mrb_fixnum_p(mrb_id))
    id = static_cast<entity_id>(mrb_fixnum(mrb_id));

  mrb_value name = mrb_hash_get(mrb, opts, name_sym_v);
  mrb_value archetype = mrb_hash_get(mrb, opts, arch_sym_v);
  if (!mrb_string_p(archetype))
    archetype = mrb_str_new_lit(mrb, "NoArchetype");

  mrb_value components = mrb_hash_get(mrb, opts, comp_sym_v);
  if (!mrb_hash_p(components))
    components = mrb_hash_new(mrb);

  entity_factory_data data;
  auto jdata = mrb_inst->hash_to_json(components).as_object();
  for (auto &item : jdata)
  {
    data[item.first] = item.second.as_object();
  }

  auto entity = EntityFactory::CreateEntity(mrb_str_to_stdstring(archetype), data, id);
  entity->Name = mrb_str_to_stdstring(name);

  GetGame()->CurrentLevel->RootEntity->AddChild(entity);

  return entity->GetRubyWrapper();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_parent(mrb_state *mrb, mrb_value self)
{
  auto *entity = ruby::data_get<Entity>(mrb, self);
  auto *parent = entity->Parent;

  return parent ? parent->RubyWrapper : mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_parent_set(mrb_state *mrb, mrb_value self)
{
  mrb_value parent_v;
  mrb_get_args(mrb, "o", &parent_v);

  auto *entity = ruby::data_get<Entity>(mrb, self);
  auto *parent = ruby::data_get<Entity>(mrb, parent_v);

  parent->AddChild(entity);
  return parent_v;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_local_event(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);

  mrb_sym event_id;
  mrb_value event_data = mrb_nil_value();
  mrb_get_args(mrb, "n|o", &event_id, &event_data);

  Events::RubyEvent data_wrapper{event_data};
  Events::EventMessage message{event_id, &data_wrapper, true};

  using namespace std::placeholders;
  Events::Event::CustomRaise(message, bind(&Entity::LocalEvent, entity, _1));

  return event_data;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_raise_event(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);

  mrb_sym event_id;
  mrb_value event_data = mrb_nil_value();
  mrb_get_args(mrb, "n|o", &event_id, &event_data);

  Events::RubyEvent data_wrapper{event_data};
  Events::EventMessage message{event_id, &data_wrapper, true};

  using namespace std::placeholders;
  Events::Event::CustomRaise(message, bind(&Entity::RaiseEvent, entity, _1));

  return event_data;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_sink_event(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);

  mrb_sym event_id;
  mrb_value event_data = mrb_nil_value();
  mrb_get_args(mrb, "n|o", &event_id, &event_data);

  Events::RubyEvent data_wrapper{event_data};
  Events::EventMessage message{event_id, &data_wrapper, true};

  using namespace std::placeholders;
  Events::Event::CustomRaise(message, bind(&Entity::SinkEvent, entity, _1));

  return event_data;
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_action_group(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);
  return mrb_actions_wrap(mrb, &entity->GetActionGroup());
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_action_sequence(mrb_state *mrb, mrb_value self)
{
  mrb_sym id;
  mrb_get_args(mrb, "n", &id);

  auto * const entity = ruby::data_get<Entity>(mrb, self);
  return mrb_actions_wrap(mrb, &entity->GetActionSequence(id));
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_zombify(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);
  entity->Zombify();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_meta_get(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);

  mrb_value mrbkey;
  mrb_get_args(mrb, "S", &mrbkey);

  mrbkey = mrb_convert_type(mrb, mrbkey, MRB_TT_STRING, "String", "to_s");

  auto value = entity->Metadata[mrb_str_to_stdstring(mrbkey)];
  return mrb_str_new(mrb, value.c_str(), value.size());
}

// ----------------------------------------------------------------------------

static mrb_value rb_ent_meta_set(mrb_state *mrb, mrb_value self)
{
  auto * const entity = ruby::data_get<Entity>(mrb, self);

  mrb_value mrbkey, mrbvalue;
  mrb_get_args(mrb, "SS", &mrbkey, &mrbvalue);
  
  mrbkey = mrb_convert_type(mrb, mrbkey, MRB_TT_STRING, "String", "to_s");
  mrbvalue = mrb_convert_type(mrb, mrbvalue, MRB_TT_STRING, "String", "to_s");

  entity->Metadata[mrb_str_to_stdstring(mrbkey)] = mrb_str_to_stdstring(mrbvalue);
  return mrbvalue;
}

// ----------------------------------------------------------------------------

ruby::ruby_class Entity::GetWrapperRClass()
{
  using namespace ruby;
  static ruby_class rclass;
  static bool init = false;

  if (init)
    return rclass;

  rb_ent_dt.struct_name = typeid(Entity).name();
  rb_ent_dt.dfree = data_nop_delete;

  rclass = ruby_engine::global_engine->define_class("GameEntity");
  rclass.define_class_method("new", mrb_nop, ARGS_ANY());

  rclass.define_method("inspect", rb_ent_inspect, ARGS_NONE());

  rclass.define_method("id", rb_ent_id, ARGS_NONE());
  rclass.define_method("name", rb_ent_name, ARGS_NONE());

  // Components
  rclass.define_method("components", rb_ent_components, ARGS_NONE());
  rclass.define_method("get_component", rb_ent_get_component, ARGS_REQ(1));
  rclass.define_method("add_component", rb_ent_add_component, ARGS_REQ(2));
  rclass.define_method("remove_component", rb_ent_remove_component, ARGS_REQ(1));

  // Proxies
  rclass.define_method("proxy_event", rb_ent_proxy_event, ARGS_REQ(3));
  rclass.define_method("remove_event", rb_ent_remove_proxy, ARGS_REQ(2));

  // Children
  rclass.define_class_method("create_entity", rb_ent_create, ARGS_OPT(1));
  rclass.define_method("children", rb_ent_children, ARGS_NONE());
  rclass.define_method("add_child", rb_ent_add_child, ARGS_REQ(1));
  rclass.define_method("remove_child", rb_ent_remove_child, ARGS_REQ(1));
  rclass.define_method("parent", rb_ent_parent, ARGS_NONE());
  rclass.define_method("parent=", rb_ent_parent_set, ARGS_REQ(1));

  rclass.define_method("find_entity", rb_ent_find_entity, ARGS_REQ(1));
  rclass.define_method("search_entities", rb_ent_search_entities, MRB_ARGS_ARG(1, 1));
  rclass.define_method("local_find", rb_ent_find_local, ARGS_REQ(1));

  // Events
  rclass.define_method("local_event", rb_ent_local_event, ARGS_REQ(1) | ARGS_OPT(1));
  rclass.define_method("raise_event", rb_ent_raise_event, ARGS_REQ(1) | ARGS_OPT(1));
  rclass.define_method("sink_event", rb_ent_sink_event, ARGS_REQ(1) | ARGS_OPT(1));

  // Actions
  rclass.define_method("action_group", rb_ent_action_group, ARGS_NONE());
  rclass.define_method("action_sequence", rb_ent_action_sequence, ARGS_REQ(1));

  // RAWR I'M A ZOMBIE!
  rclass.define_method("zombify!", rb_ent_zombify, ARGS_NONE());

  // Metadata
  rclass.define_method("[]", rb_ent_meta_get, ARGS_REQ(1));
  rclass.define_method("[]=", rb_ent_meta_set, ARGS_REQ(2));

  return rclass;
}

// ----------------------------------------------------------------------------

static BucketAllocator entityAllocator{sizeof(Entity)};

// ----------------------------------------------------------------------------

std::unordered_map<std::string, std::vector<std::string>> &GetComponentDependencies()
{
  static std::unordered_map<std::string, std::vector<std::string>> items;
  static bool init = false;
  if (init)
    return items;

  auto jlist = ParseJsonAsset("Definitions", "ComponentDependencies.json");
  for (auto &pair : jlist.as_object())
  {
    items[pair.first] = pair.second.as_array_of<json::value::string_t>();
  }

  init = true;
  return items;
}

// ----------------------------------------------------------------------------

/**
  Whether b depends on a
*/
static bool component_depends_on(const std::string &a, const std::string &b)
{
  auto &dependencies = GetComponentDependencies();
  auto deps = dependencies.find(b);
  if (deps == dependencies.end())
    return false;

  auto &deplist = deps->second;
  return find(deplist.begin(), deplist.end(), a) != deplist.end();
}

// ----------------------------------------------------------------------------

typedef std::pair<std::string, component_factory_data> cpair;

bool ComponentsAreSorted(std::vector<cpair> &components, size_t &i, size_t &j)
{
  for (i = 0; i < components.size(); ++i)
  {
    for (j = i + 1; j < components.size(); ++j)
    {
      if (component_depends_on(components[j].first, components[i].first))
        return false;
    }
  }

  return true;
}

// ----------------------------------------------------------------------------

static std::vector<std::pair<std::string, component_factory_data>>
SortComponentDependencies(const entity_factory_data &data)
{
  std::vector<cpair> components;
  components.reserve(data.size());

  // copy the components over
  for (auto &comp : data)
  {
    components.push_back(comp);
  }

  size_t i, j;
  while (!ComponentsAreSorted(components, i, j))
  {
    swap(components[i], components[j]);
  }

  return components;
}

// ----------------------------------------------------------------------------

Entity *EntityFactory::CreateEntity(const std::string &entdef,
                                    const entity_factory_data &data,
                                    entity_id entid)
{
  // Read entdef
  entity_factory_data entdata;
  {
    auto tree = ParseJsonAsset("Entities", entdef + ".entitydef");
    if (!tree.is_object_of<json::value::object_t>())
      throw basic_exception("Incorrect entity definition file");

    auto parts = tree.as_object_of<json::value::object_t>();
    for (auto &part : parts)
    {
      auto &datamap = entdata[part.first];

      for (auto &prop : part.second)
      {
        datamap[prop.first] = prop.second;
      }
    }
  }

  // Merge entdef with data
  for (auto &part : data)
  {
    auto &datamap = entdata[part.first];

    for (auto &prop : part.second)
    {
      datamap[prop.first] = prop.second;
    }
  }

  auto entity = entityAllocator.Create<Entity>(entid);
  auto components = SortComponentDependencies(entdata);

  // Create all of the components
  for (auto &cpair : components)
  {
    entity->AddComponent(cpair.first, cpair.second);
  }

  mrb_full_gc(*mrb_inst);
  return entity;
}

// ----------------------------------------------------------------------------

void EntityFactory::DestroyEntity(Entity *entity)
{
  // Let it be known that this thing is gone
  {
    static Events::EventId recieverDestroyedId("event_reciever_destroyed");
    Events::EventRecieverDestroyedEvent data{entity};
    Events::EventMessage message{recieverDestroyedId, &data, false};
    Events::Event::Raise(message);
  }

  entityAllocator.Destroy(entity);
}

// ----------------------------------------------------------------------------

void Entity::ApplyParentTransforms()
{
  if (Parent)
  {
    Transform = LocalTransform * Parent->Transform;
  }
  else
  {
    Transform = LocalTransform;
  }
}

// ----------------------------------------------------------------------------

void Entity::OnUpdate(float dt)
{
  _actionGroup.Update(dt);
  for (auto &pair : _actionSequences)
  {
    pair.second->Update(dt);
  }
}

// ----------------------------------------------------------------------------

ActionManager &Entity::GetActionGroup()
{
  return _actionGroup;
}

// ----------------------------------------------------------------------------

ActionManager &Entity::GetActionSequence(mrb_sym id)
{
  auto seq = _actionSequences[id];
  if (!seq)
  {
    _actionSequences[id] = seq = sequenceAllocator.Create<ActionSequence>();
  }

  return *seq;
}

// ----------------------------------------------------------------------------

ActionManager &Entity::GetActionSequence(const char *name)
{
  return GetActionSequence(mrb_intern_cstr(*mrb_inst, name));
}

// ----------------------------------------------------------------------------

void Entity::Zombify()
{
  // You better not zombify LevelRoot, or Connor is coming after you :U (LevelRoot is id: 0)
  if (Id == 0)
    mrb_raise(*mrb_inst, mrb_inst->mrb_handle()->eException_class,
              "Zombifying LevelRoot is UNACCEPTABLEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
              "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");

  if (zombified)
    return;

  zombified = true;

  // Zombified event
  {
    DEF_EVENT_ID(zombified);
    Events::EventMessage message(zombified, nullptr, false);
    LocalEvent(message);
  }

  death_row.push_front(this);

  while (!children.empty())
    children.front()->Zombify();

  this->RecalculateEventCounts();

  if (Parent)
    Parent->RemoveChild(this);
}

// ----------------------------------------------------------------------------

void Entity::ExecuteZombies()
{
  while (!death_row.empty())
  {
    auto *zombie = death_row.front();
    death_row.pop_front();

    EntityFactory::DestroyEntity(zombie);
  }
}

// ----------------------------------------------------------------------------