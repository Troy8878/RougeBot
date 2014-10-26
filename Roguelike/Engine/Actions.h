/*********************************
 * Actions.h
 * Connor Hilarides
 * Created 2014/10/24
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

  virtual ~Action() {};

protected:
  Action() = default;
};

// ----------------------------------------------------------------------------

class ActionManager
{
public:
  virtual void Update(float dt) = 0;
  virtual void Queue(Action *action) = 0;

protected:
  ~ActionManager() {}
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
  DelayAction(double time);
  bool Update(float dt) override;

private:
  double _time;
};

// ----------------------------------------------------------------------------

class FunctionalAction : public Action
{
public:
  typedef std::function<bool(float dt)> UpdateFunc;

  FunctionalAction(const UpdateFunc& func);
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

class VectorEaseAction : public Action
{
public:
  enum EasingMode
  {
    EASE_LINEAR = 1,
    EASE_EXPONENTIAL,
    EASE_BOUNCE,
  };

  VectorEaseAction(math::Vector *vect, math::Vector end,
                   double time, EasingMode mode);
  
  bool Update(float dt) override;

private:
  bool UpdateLinear(float dt);
  bool UpdateExponential(float dt);
  bool UpdateBounce(float dt);

  math::Vector *_vect;
  math::Vector _begin;
  math::Vector _end;
  math::Vector _offset;
  double _time;
  double _curr_time;
  EasingMode _ease;

  static const unsigned BOUNCES_MAX = 10;
  static const float BOUNCE_EPSILON;
  static const float BOUNCE_HEIGHT;

  union EaseData
  {
    struct BounceData
    {
      int bounces;
      float elasticity;
      float acceleration;
      float durations[10];
      float heights[10];
      float velocities[10];
      float duration;

      float value_at(float progress);

    } bounce_data;
  } ed;
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
  return (_object->*_func)(dt);
}

#pragma endregion

// ----------------------------------------------------------------------------


