/*********************************
 * Actions.h
 * Connor Hilarides
 * Created 2014/10/24
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

class ActionManager;

mrb_value mrb_actions_wrap(mrb_state *mrb, ActionManager *manager);
ActionManager *mrb_actions_unwrap(mrb_state *mrb, mrb_value manager);

// ----------------------------------------------------------------------------

class Action
{
public:
  NO_COPY_CONSTRUCTOR(Action);
  NO_ASSIGNMENT_OPERATOR(Action);

  /**
    Update this action in the sequence,
    returning false if this action has completed
  */
  virtual bool Update(float dt) = 0;

  virtual ~Action()
  {
  };

protected:
  Action() = default;
};

// ----------------------------------------------------------------------------

class ActionManager
{
public:
  virtual ~ActionManager()
  {
  }

  virtual void Update(float dt) = 0;
  virtual void Queue(Action *action) = 0;
};

// ----------------------------------------------------------------------------

class ActionGroup : public ActionManager
{
public:
  ~ActionGroup();

  void Update(float dt) override;
  void Queue(Action *action) override;

private:
  std::vector<Action *> _actions;
};

// ----------------------------------------------------------------------------

class ActionSequence : public ActionManager
{
public:
  ~ActionSequence();

  void Update(float dt) override;
  void Queue(Action *action) override;

private:
  std::queue<Action *> _actions;
};

// ----------------------------------------------------------------------------

class DelayAction : public Action
{
public:
  explicit DelayAction(double time);
  bool Update(float dt) override;

private:
  double _time;
};

// ----------------------------------------------------------------------------

class FunctionalAction : public Action
{
public:
  typedef std::function<bool(float dt)> UpdateFunc;

  explicit FunctionalAction(const UpdateFunc &func);
  bool Update(float dt) override;

private:
  UpdateFunc _func;
};

// ----------------------------------------------------------------------------

template <typename Class>
class MemberFunctionAction : public Action
{
public:
  typedef bool(Class::*UpdateFunc)(float dt);

  MemberFunctionAction(Class *object, UpdateFunc func);
  bool Update(float dt) override;

private:
  Class *_object;
  UpdateFunc _func;
};

// ----------------------------------------------------------------------------

class RubyProcAction : public Action
{
public:
  RubyProcAction(mrb_value proc);
  ~RubyProcAction();

  bool Update(float dt) override;

private:
  mrb_value _proc;
  size_t _gc_hold_id;
};

// ----------------------------------------------------------------------------

class RubyObjectAction : public Action
{
public:
  RubyObjectAction(mrb_value obj, mrb_sym method);
  ~RubyObjectAction();

  bool Update(float dt) override;

private:
  mrb_value _obj;
  mrb_sym _method;
  size_t _gc_hold_id;
};

// ----------------------------------------------------------------------------

class VectorInterpolateAction : public Action
{
public:
  explicit VectorInterpolateAction(math::Vector &vector, const math::Vector &end, 
                                   double time, bool deferBegin = false);
  bool Update(float dt) override;

private:
  math::Vector &vector;
  math::Vector begin;
  math::Vector diff;
  double time;
  double totaltime;
  bool deferBegin;
};

// ----------------------------------------------------------------------------

#pragma region MemberFunctionAction

template <typename Class>
MemberFunctionAction<Class>::MemberFunctionAction(Class *object, UpdateFunc func)
  : _object(object), _func(func)
{
}

// ----------------------------------------------------------------------------

template <typename Class>
bool MemberFunctionAction<Class>::Update(float dt)
{
  return (_object ->* _func)(dt);
}

#pragma endregion

// ----------------------------------------------------------------------------