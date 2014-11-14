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

#pragma region VectorEaseAction

const float VectorEaseAction::BOUNCE_EPSILON = 1.0f;
const float VectorEaseAction::BOUNCE_HEIGHT = 1000.0f;

VectorEaseAction::VectorEaseAction(math::Vector *vect, math::Vector end,
                                   double time, EasingMode mode)
  : _vect(vect), _begin(*vect), _end(end), _offset(_end - _begin),
    _time(time), _curr_time(0), _ease(mode)
{
#pragma region Initialize Bouncing
  // Initialize bounce easing
  if (_ease == EASE_BOUNCE)
  {
    // Hardcoded for now, maybe I'll find a way to fix that later
    const float elasticity = 0.1f;
    auto &bd = ed.bounce_data;

    bd.elasticity = elasticity;
    bd.bounces =
      int(log(BOUNCE_EPSILON / BOUNCE_HEIGHT) / log(elasticity));

    float totalDuration = 0.0f;
    float height = BOUNCE_HEIGHT;

    // Calculate all of the relative sizes of the bounces
    for (int i = 0; i < bd.bounces; ++i)
    {
      float duration = sqrt(2 * height) * 2;

      if (i == 0)
        duration /= 2;

      bd.durations[i] = duration;
      bd.heights[i] = height;

      totalDuration += duration;
      height *= bd.elasticity;
    }

    // Calculate the total duration
    bd.duration = 0.0f;
    for (int i = 0; i < bd.bounces; ++i)
    {
      float duration = float(bd.durations[i] * _time / totalDuration);
      bd.durations[i] = duration;
      bd.duration += duration;
    }

    // Calculate acceleration
    float firstHalfDuration = bd.durations[0];
    bd.acceleration =
      (2.0f * BOUNCE_HEIGHT) / (firstHalfDuration * firstHalfDuration);

    // Calculate Velocities
    for (int i = 0; i < bd.bounces; ++i)
    {
      if (i == 0)
        bd.velocities[i] = 0.0f;
      else
        bd.velocities[i] =
          bd.durations[i] / 2.0f * bd.acceleration;
    }

    // Now invert the acceleration to point "down"
    bd.acceleration = -bd.acceleration;
  }
#pragma endregion
}

// ----------------------------------------------------------------------------

bool VectorEaseAction::Update(float dt)
{
  switch (_ease)
  {
  case EASE_LINEAR:
    return UpdateLinear(dt);

  case EASE_EXPONENTIAL:
    return UpdateExponential(dt);

  case EASE_BOUNCE:
    return UpdateBounce(dt);
  }

  return false;
}

// ----------------------------------------------------------------------------

bool VectorEaseAction::UpdateLinear(float dt)
{
  _curr_time += dt;
  if (_curr_time >= _time)
  {
    *_vect = _end;
    return false;
  }

  *_vect = _begin + _offset * float(_curr_time / _time);
  return true;
}

// ----------------------------------------------------------------------------

bool VectorEaseAction::UpdateExponential(float dt)
{
  _curr_time += dt;
  if (_curr_time >= _time)
  {
    *_vect = _end;
    return false;
  }

  *_vect = _begin + _offset * float(log(_curr_time + 1) / log(_time + 1));
  return true;
}

// ----------------------------------------------------------------------------

bool VectorEaseAction::UpdateBounce(float dt)
{
  _curr_time += dt;
  if (_curr_time >= _time)
  {
    *_vect = _end;
    return false;
  }

  float time = ed.bounce_data.value_at(float(_curr_time));
  *_vect = _begin + _offset * time;
  return true;
}

// ----------------------------------------------------------------------------

float VectorEaseAction::EaseData::BounceData::value_at(float progress)
{
  if (progress < 0)
    progress = 0;
  else if (progress > duration)
    progress = duration;

  int index = 0;
  float total = 0;

  while (index < bounces && progress > total + durations[index])
    total += durations[index++];

  progress -= total;

  float height;
  if (index == 0)
  {
    height = BOUNCE_HEIGHT + acceleration * (progress * progress) / 2.0f;
  }
  else
  {
    height = progress * (velocities[index] + (acceleration * progress) / 2.0f);
  }

  return height / BOUNCE_HEIGHT;
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

    mrb_sym lock_sym = mrb_intern_lit(*mrb_inst, "ActionSequenceLock");
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