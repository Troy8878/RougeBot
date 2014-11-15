/*********************************
 * Entity.h
 * Connor Hilarides
 * Created 2014/08/11
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "EventHandlers.h"
#include "Actions.h"

// ----------------------------------------------------------------------------

class Component;

typedef uint64_t entity_id;

// ----------------------------------------------------------------------------

const entity_id UNASSIGNED_ENTITY_ID = std::numeric_limits<entity_id>::max();

// ----------------------------------------------------------------------------

class Entity;

struct EventProxyList
{
  typedef std::function<void(Events::EventMessage &)> Func;
  std::unordered_map<Entity *, Func> maps;
};

// ----------------------------------------------------------------------------

class Entity : public Events::EventReciever
{
#pragma region Typedefs

public:

  typedef void(Component::*component_handler)(Events::EventMessage &);

  template <typename T>
  using derived_handler = void(T::*)(Events::EventMessage &);

  typedef std::unordered_map<Component *, component_handler> event_registrations;

#pragma endregion

#pragma region Constructors and Properties

public:

  Entity(entity_id id = UNASSIGNED_ENTITY_ID);

  // Entities are going to have way too many references to move
  NO_COPY_CONSTRUCTOR(Entity);
  NO_ASSIGNMENT_OPERATOR(Entity);

  PROPERTY(get = _GetEntityId) entity_id Id;
  PROPERTY(get = GetRubyWrapper) mrb_value RubyWrapper;

  PROPERTY(get = _GetName, put = _SetName) std::string Name;
  IRW_PROPERTY(math::Matrix, LocalTransform);
  IR_PROPERTY(math::Matrix, Transform);

  void ApplyParentTransforms();

private:
  ~Entity();

  std::string _name;

  friend class EntityFactory;
  friend class BucketAllocator;

public:
  std::string &_GetName()
  {
    return _name;
  }

  const std::string &_GetName() const
  {
    return _name;
  }

  void _SetName(const std::string &name);

#pragma endregion

#pragma region Components

public:

  /**
    Initialize a new component for this entity with the
    given name and component factory data
  */
  Component *AddComponent(const std::string &name, component_factory_data &data);

  /**
    Destruct the component with the given name from this
    entity
  */
  void RemoveComponent(const std::string &name);

  /**
    Returns the component on this entity with the given name
  */
  Component *GetComponent(const std::string &name);

  template <typename DerivedComponent>
  DerivedComponent *GetComponent(const std::string &name)
  {
#ifdef _DEBUG
    return dynamic_cast<DerivedComponent *>(GetComponent(name));
#else
    return static_cast<DerivedComponent *>(GetComponent(name));
#endif
  }

  PROPERTY(get = _GetComponents) const std::unordered_map<std::string, Component *> &Components;

#pragma endregion

#pragma region Events

public:

  /**
    Check if any of your components care about
    the eventId of this event message.
  */
  bool CanHandle(const Events::EventMessage &e) override;
  /**
    Dispatch this event to all of your components
    that are waiting for it.
  */
  void Handle(Events::EventMessage &e) override;

  void HandleComponents(Events::EventMessage &e);
  void HandleProxies(Events::EventMessage &e);

  void LocalEvent(Events::EventMessage &e);
  void RaiseEvent(Events::EventMessage &e);
  void SinkEvent(Events::EventMessage &e);

  /**
    This will be used when one of your components wants to
    connect to an event
  */
  void AddEvent(Component *component, event_id id, component_handler handler);
  /**
    This will be used when one of your components no longer wants
    to recieve an event
  */
  void RemoveEvent(Component *component, event_id id);

  void AddProxy(Entity *entity, event_id id, EventProxyList::Func func);
  void RemoveProxy(Entity *entity, event_id id);

  void HandleSend(Events::EventMessage &e);

  /**
    Helper for adding member functions of properly inheriting components
  */
  template <typename Derived>
  void AddEvent(Component *component, event_id id, derived_handler<Derived> handler)
  {
    AddEvent(component, id, static_cast<component_handler>(handler));
  }

  void RecalculateEventCounts();

private:
  std::unordered_map<event_id, EventProxyList> proxies;
  bool proxies_invalidated;

#pragma endregion

#pragma region Children and Parents

public:
  IR_PROPERTY(Entity *, Parent);

  /**
    Adds a child entity to this entity.
    Will remove the entity from a former parent if one exists.
  */
  void AddChild(Entity *entity);
  /**
    Removes a child entity from this parent. Be aware
    that if the entity is abondoned, it will have been
    leaked. Ensure it is either added to another parent,
    or properly destroyed via the EntityFactory.
  */
  void RemoveChild(Entity *entity);

  /**
    Finds the exact child (or self) entity with
    this id, or nullptr if it is not this or a child entity
  */
  Entity *FindEntity(entity_id id);
  /**
    Find an entity by exact match of a name.
    This search is case sensitive.
  */
  Entity *FindEntity(const std::string &name);

  Entity *LocalFind(const std::string &name);

  /**
    Finds entities by a full or partial match on their name.
  */
  void SearchEntities(std::vector<Entity *> &results,
                      const std::string &namePattern,
                      bool partialMatch = true);
  /**
    Finds entities by a partial match on their name.
    This search uses the passed regex for matching.
  */
  void SearchEntities(std::vector<Entity *> &results,
                      const std::regex &namePattern);

  PROPERTY(get = _GetChildren) const std::vector<Entity *> &Children;

  const std::vector<Entity *> &_GetChildren()
  {
    return children;
  }

  bool IsSelfOrChildOf(Entity *other)
  {
    if (this == other)
      return true;
    if (Parent == nullptr)
      return false;

    return Parent->IsSelfOrChildOf(other);
  }

private:
  void DestroyChildren();

  void RegisterNamehash();
  void UnregisterNamehash();

  std::vector<Entity *> children;

#pragma endregion

#pragma region Protected fields

protected:

  /**
    Keep track of all your components :)
  */
  std::unordered_map<std::string, Component *> _components;

  /**
    Store all of your registered events here, keyed
    on the event id for fast lookup ;)
  */
  std::unordered_map<event_id, event_registrations> _events;

  std::unordered_map<event_id, size_t> _eventCounts;

public:
  auto _GetComponents() -> const std::remove_reference<decltype(_components)>::type&
  {
    return _components;
  }

#pragma endregion

#pragma region Other fields and helpers

public:
  entity_id _GetEntityId()
  {
    return _id;
  }

  mrb_value GetRubyWrapper();

private:
  entity_id _id;

  void OnUpdate(float dt);

  static entity_id CreateEntityId();
  static ruby::ruby_class GetWrapperRClass();

  friend static mrb_value rb_ent_inspect(mrb_state *mrb, mrb_value self);
  friend static mrb_value rb_ent_components(mrb_state *mrb, mrb_value self);

#pragma endregion

#pragma region Actions

public:
  ActionManager &GetActionGroup();
  ActionManager &GetActionSequence(mrb_sym id);
  ActionManager &GetActionSequence(const char *name);

private:
  ActionGroup _actionGroup;
  std::unordered_map<mrb_sym, ActionSequence *> _actionSequences;

#pragma endregion

#pragma region Orphans

public:
  // Adds the entity to Death Row, and will be deleted next frame
  void Zombify();
  // Deletes all entities on Death Row (in the death_row vector)
  static void ExecuteZombies();

private:
  bool zombified = false;

#pragma endregion
};

// ----------------------------------------------------------------------------

typedef std::unordered_map<std::string, component_factory_data> entity_factory_data;

class EntityFactory
{
public:
  static Entity *CreateEntity(const std::string &entdef,
                              const entity_factory_data &data,
                              entity_id entid = UNASSIGNED_ENTITY_ID);
  static void DestroyEntity(Entity *entity);
};

// ----------------------------------------------------------------------------

std::unordered_map<std::string, std::vector<std::string>> &GetComponentDependencies();

// ----------------------------------------------------------------------------