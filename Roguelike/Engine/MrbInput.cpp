/*********************************
* MrbInput.cpp
* Connor Hilarides
* Created 2014/10/21
*********************************/

#include "Common.h"
#include "Input.h"
#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

#define DEF_INT_CONST(lit, val) mrb_define_const(mrb, rclass, lit, mrb_fixnum_value(val))

static mrb_data_type mrb_kse_data_type;

static mrb_value mrb_kse_new(mrb_state *mrb, const KeyState *kse);
static void mrb_kse_free(mrb_state *mrb, void *mem);

static mrb_value mrb_kse_is_held(mrb_state *mrb, mrb_value self);
static mrb_value mrb_kse_hold_time(mrb_state *mrb, mrb_value self);
static mrb_value mrb_kse_vkey(mrb_state *mrb, mrb_value self);
static mrb_value mrb_kse_is_plain_char(mrb_state *mrb, mrb_value self);
static mrb_value mrb_kse_plain_char(mrb_state *mrb, mrb_value self);

static mrb_data_type mrb_me_data_type;

static mrb_value mrb_me_new(mrb_state *mrb, const MouseState *state);
static void mrb_me_free(mrb_state *mrb, void *mem);

static mrb_value mrb_me_position(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_keystate_init(mrb_state *mrb)
{
  mrb_kse_data_type.dfree = mrb_kse_free;
  mrb_kse_data_type.struct_name = typeid(KeyState).name();

  RClass *rclass = mrb_define_class(mrb, "KeyState", mrb->object_class);

  // ruby can't make new ones, only Input can
  mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_ANY());

  mrb_define_method(mrb, rclass, "held?", mrb_kse_is_held, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "hold_time", mrb_kse_hold_time, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "vkey", mrb_kse_vkey, ARGS_NONE());
  mrb_define_method(mrb, rclass, "plain_char?", mrb_kse_is_plain_char, ARGS_NONE());
  mrb_define_method(mrb, rclass, "plain_char", mrb_kse_plain_char, ARGS_NONE());

  mrb_me_data_type.dfree = mrb_me_free;
  mrb_me_data_type.struct_name = "MouseState";

  rclass = mrb_define_class(mrb, "MouseState", mrb->object_class);

  // ruby can't make new ones, only Input can
  mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_ANY());

  mrb_define_method(mrb, rclass, "position", mrb_me_position, ARGS_NONE());
}

// ----------------------------------------------------------------------------

mrb_value KeyStateEvent::GetRubyWrapper()
{
  return mrb_kse_new(*mrb_inst, state);
}

// ----------------------------------------------------------------------------

mrb_value MouseEvent::GetRubyWrapper()
{
  return mrb_me_new(*mrb_inst, state);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_new(mrb_state *mrb, const KeyState *kse)
{
  static auto rclass = mrb_class_get(mrb, "KeyState");

  auto *ptr = const_cast<KeyState *>(kse);
  auto data = mrb_data_object_alloc(mrb, rclass, ptr, &mrb_kse_data_type);

  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------

static void mrb_kse_free(mrb_state *, void *)
{
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_is_held(mrb_state *mrb, mrb_value self)
{
  auto key = static_cast<const KeyState *>(mrb_data_get_ptr(mrb, self, &mrb_kse_data_type));
  return mrb_bool_value(key->down);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_hold_time(mrb_state *mrb, mrb_value self)
{
  auto key = static_cast<const KeyState *>(mrb_data_get_ptr(mrb, self, &mrb_kse_data_type));
  return mrb_float_value(mrb, key->hold_time);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_vkey(mrb_state *mrb, mrb_value self)
{
  auto key = static_cast<const KeyState *>(mrb_data_get_ptr(mrb, self, &mrb_kse_data_type));
  return mrb_fixnum_value(key->virtual_key);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_is_plain_char(mrb_state *mrb, mrb_value self)
{
  auto key = static_cast<const KeyState *>(mrb_data_get_ptr(mrb, self, &mrb_kse_data_type));
  return mrb_bool_value(key->char_code >= 0x20 && key->char_code <= 0x7F);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_plain_char(mrb_state *mrb, mrb_value self)
{
  auto key = static_cast<const KeyState *>(mrb_data_get_ptr(mrb, self, &mrb_kse_data_type));
  if (key->char_code < 0x20 || key->char_code > 0x7F)
    return mrb_nil_value();

  char c[] = {static_cast<char>(key->char_code), 0};
  return mrb_str_new_cstr(mrb, c);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_me_new(mrb_state *mrb, const MouseState *state)
{
  auto rclass = mrb_class_get(mrb, "MouseState");

  auto *ptr = const_cast<MouseState *>(state);
  auto data = mrb_data_object_alloc(mrb, rclass, ptr, &mrb_me_data_type);

  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------

static void mrb_me_free(mrb_state *, void *)
{
}

// ----------------------------------------------------------------------------

static mrb_value mrb_me_position(mrb_state *mrb, mrb_value self)
{
  const MouseState *state = static_cast<const MouseState *>(mrb_data_get_ptr(mrb, self, &mrb_me_data_type));

  return ruby::create_new_vector(state->position.get());
}

// ----------------------------------------------------------------------------