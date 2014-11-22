/*********************************
 * MrbActions.cpp
 * Connor Hilarides
 * Created 2014/10/25
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

mrb_data_type mrb_actions_dt;
static RClass *actions_c;

static mrb_value mrb_actions_delay(mrb_state *mrb, mrb_value self);
static mrb_value mrb_actions_enqueue(mrb_state *mrb, mrb_value self);
static mrb_value mrb_actions_interpolate(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_actions_init(mrb_state *mrb)
{
  mrb_actions_dt.dfree = ruby::data_nop_delete;
  mrb_actions_dt.struct_name = typeid(ActionManager).name();

  auto actions = actions_c = mrb_define_class(mrb, "ActionManager", mrb->object_class);
  mrb_define_class_method(mrb, actions, "new", mrb_nop, ARGS_ANY());

  mrb_define_method(mrb, actions, "delay", mrb_actions_delay, ARGS_REQ(1));
  mrb_define_method(mrb, actions, "enqueue", mrb_actions_enqueue, ARGS_REQ(1));
  mrb_define_method(mrb, actions, "interpolate", mrb_actions_interpolate, ARGS_ANY());
}

// ----------------------------------------------------------------------------

mrb_value mrb_actions_wrap(mrb_state *mrb, ActionManager *manager)
{
  auto datap = reinterpret_cast<void *>(manager);
  auto obj = mrb_data_object_alloc(mrb, actions_c, datap, &mrb_actions_dt);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

ActionManager *mrb_actions_unwrap(mrb_state *mrb, mrb_value manager)
{
  auto datap = mrb_data_get_ptr(mrb, manager, &mrb_actions_dt);
  return reinterpret_cast<ActionManager *>(datap);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_actions_delay(mrb_state *mrb, mrb_value self)
{
  mrb_float time;
  mrb_get_args(mrb, "f", &time);

  auto *manager = mrb_actions_unwrap(mrb, self);

  manager->Queue(new DelayAction(time));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_actions_enqueue(mrb_state *mrb, mrb_value self)
{
  static auto terr = mrb_class_get(mrb, "TypeError");

  mrb_value obj, block = mrb_nil_value();
  mrb_sym method;
  mrb_bool obj_p, method_p;
  mrb_get_args(mrb, "|o?n?&", &obj, &obj_p, &method, &method_p, &block);

  auto *manager = mrb_actions_unwrap(mrb, self);

  if (!mrb_nil_p(block)) // Store block as a proc
  {
    manager->Queue(new RubyProcAction(block));
  }
  else if (obj_p && !method_p) // Store obj as a proc
  {
    if (obj.tt != MRB_TT_PROC)
      mrb_raise(mrb, terr, "ActionManager#enqueue with 1 argument must be given a Proc");

    manager->Queue(new RubyProcAction(obj));
  }
  else if (obj_p && method_p)
  {
    manager->Queue(new RubyObjectAction(obj, method));
  }
  else
  {
    mrb_raise(mrb, terr,
              "ActionManager#enqueue has the following overloads:"
              "  - enqueue {|dt| block }"
              "  - enqueue(Proc)"
              "  - enqueue(Object, Symbol)");
  }

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

mrb_value mrb_actions_interpolate(mrb_state* mrb, mrb_value self)
{
  static mrb_sym sym_over = mrb_intern_lit(mrb, "over");
  static mrb_sym sym_from = mrb_intern_lit(mrb, "from");
  static mrb_sym sym_to = mrb_intern_lit(mrb, "to");
  static mrb_sym sym_by = mrb_intern_lit(mrb, "by");

  math::Vector *val_vector;
  mrb_value opts;
  mrb_get_args(mrb, "dH", &val_vector, &ruby::mrb_vector_type, &opts);

  if (!mrb_hash_p(opts))
    mrb_raise(mrb, E_TYPE_ERROR, "Options must be a Hash");

  math::Vector &vector = *val_vector;
  math::Vector end = vector;

  #pragma region Interpolate Over

  mrb_value val_over = mrb_hash_get(mrb, opts, mrb_symbol_value(sym_over));
  val_over = mrb_convert_type(mrb, val_over, MRB_TT_FLOAT, "Float", "to_f");
  mrb_float over = mrb_float(val_over);
  if (!mrb_float_p(val_over))
    mrb_raise(mrb, E_ARGUMENT_ERROR, 
              "argument ':over' must be specified and be a fixnum/float");

  #pragma endregion

  #pragma region Interpolate From

  mrb_value val_from = mrb_hash_get(mrb, opts, mrb_symbol_value(sym_from));
  if (val_from.tt == MRB_TT_DATA)
    vector = ruby::get_ruby_vector(val_from);

  #pragma endregion

  #pragma region Interpolate To

  mrb_value val_to = mrb_hash_get(mrb, opts, mrb_symbol_value(sym_to));
  if (val_to.tt == MRB_TT_DATA)
    end = ruby::get_ruby_vector(val_to);

  #pragma endregion

  #pragma region Interpolate By

  mrb_value val_by = mrb_hash_get(mrb, opts, mrb_symbol_value(sym_by));
  if (val_by.tt == MRB_TT_DATA)
    end = vector + ruby::get_ruby_vector(val_by);

  #pragma endregion

  auto *manager = mrb_actions_unwrap(mrb, self);
  manager->Queue(new VectorInterpolateAction(vector, end, over));
  return self;
}

// ----------------------------------------------------------------------------