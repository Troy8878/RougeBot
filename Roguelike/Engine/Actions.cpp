/*********************************
 * Actions.cpp
 * Connor Hilarides
 * Created 2014/10/24
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Actions.h"

// ----------------------------------------------------------------------------

EXTERN_C mrb_int GCLockObj(mrb_value value);
EXTERN_C void GCUnlockObj(mrb_int holdId);

// ----------------------------------------------------------------------------

#pragma region ActionGroup

ActionGroup::~ActionGroup()
{
  for (auto *action : _actions)
  {
    delete action;
  }
}

// ----------------------------------------------------------------------------

void ActionGroup::Update(float dt)
{
  if (_actions.empty())
    return;

  for (auto it = _actions.begin(); it != _actions.end();)
  {
    auto *action = *it;

    if (action->Update(dt))
    {
      ++it;
    }
    else
    {
      delete action;
      it = _actions.erase(it);
    }
  }
}

// ----------------------------------------------------------------------------

void ActionGroup::Queue(Action *action)
{
  _actions.push_back(action);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region ActionSequence

ActionSequence::~ActionSequence()
{
  while (!_actions.empty())
  {
    delete _actions.front();
    _actions.pop();
  }
}

// ----------------------------------------------------------------------------

void ActionSequence::Update(float dt)
{
  if (_actions.empty())
    return;

  auto *action = _actions.front();

  if (action->Update(dt) == false)
  {
    delete action;
    _actions.pop();
  }
}

// ----------------------------------------------------------------------------

void ActionSequence::Queue(Action *action)
{
  _actions.push(action);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region DelayAction

DelayAction::DelayAction(double time)
  : _time(time)
{
}

// ----------------------------------------------------------------------------

bool DelayAction::Update(float dt)
{
  _time -= dt;
  return _time > 0;
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region FunctionalAction

FunctionalAction::FunctionalAction(const UpdateFunc &func)
  : _func(func)
{
}

// ----------------------------------------------------------------------------

bool FunctionalAction::Update(float dt)
{
  return _func(dt);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region RubyProcAction

RubyProcAction::RubyProcAction(mrb_value proc)
  : _proc(proc)
{
  _gc_hold_id = GCLockObj(_proc);
}

// ----------------------------------------------------------------------------

RubyProcAction::~RubyProcAction()
{
  GCUnlockObj(_gc_hold_id);
}

// ----------------------------------------------------------------------------

bool RubyProcAction::Update(float dt)
{
  static mrb_sym call = mrb_intern_lit(*mrb_inst, "call");

  mrb_value dt_value = mrb_float_value(*mrb_inst, dt);
  mrb_value result = mrb_funcall_argv(*mrb_inst, _proc, call, 1, &dt_value);

  if (mrb_inst->mrb_handle()->exc)
  {
    mrb_inst->log_and_clear_error();
    return false;
  }

  return result.tt != MRB_TT_FALSE;
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region RubyObjectAction

RubyObjectAction::RubyObjectAction(mrb_value obj, mrb_sym method)
  : _obj(obj), _method(method)
{
  _gc_hold_id = GCLockObj(_obj);
}

// ----------------------------------------------------------------------------

RubyObjectAction::~RubyObjectAction()
{
  GCUnlockObj(_gc_hold_id);
}

// ----------------------------------------------------------------------------

bool RubyObjectAction::Update(float dt)
{
  mrb_value dt_value = mrb_float_value(*mrb_inst, dt);
  mrb_value result = mrb_funcall_argv(*mrb_inst, _obj, _method, 1, &dt_value);

  if (mrb_inst->mrb_handle()->exc)
  {
    mrb_inst->log_and_clear_error();
    return false;
  }

  return result.tt != MRB_TT_FALSE;
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Mruby GC Lock

static mrb_value GCLockHash()
{
  static bool init = false;
  static mrb_value lock = mrb_hash_new(*mrb_inst);

  if (!init)
  {
    init = true;

    mrb_sym lock_sym = mrb_intern_lit(*mrb_inst, "$PancakeEngineLock");
    mrb_gv_set(*mrb_inst, lock_sym, lock);
  }

  return lock;
}

// ----------------------------------------------------------------------------

EXTERN_C mrb_int GCLockObj(mrb_value value)
{
  static mrb_int next_lock_id = 0;
  static auto lock = GCLockHash();
  auto lock_id = next_lock_id++;

  mrb_value lock_v = mrb_fixnum_value(lock_id);

  mrb_hash_set(*mrb_inst, lock, lock_v, value);

  return lock_id;
}

// ----------------------------------------------------------------------------

EXTERN_C void GCUnlockObj(mrb_int holdId)
{
  static auto lock = GCLockHash();
  mrb_value lock_v = mrb_fixnum_value(holdId);

  mrb_hash_delete_key(*mrb_inst, lock, lock_v);
}

#pragma endregion

// ----------------------------------------------------------------------------

VectorInterpolateAction::VectorInterpolateAction
(
  math::Vector &vector,
  const math::Vector &end,
  double time, 
  bool deferBegin
) : vector(vector), begin(vector), diff(end - vector),
    time(0), totaltime(time), deferBegin(deferBegin)
{
}

// ----------------------------------------------------------------------------

bool VectorInterpolateAction::Update
(
  float dt
)
{
  if (deferBegin)
  {
    deferBegin = false;
    begin = vector;
  }

  time += dt;
  vector = begin + diff * static_cast<float>(time / totaltime);
  return time < totaltime;
}

// ----------------------------------------------------------------------------