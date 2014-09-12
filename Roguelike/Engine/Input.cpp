/*********************************
* Input.cpp
* Connor Hilarides
* Created 2014/09/08
*********************************/

#include "Common.h"
#include "Input.h"

// ----------------------------------------------------------------------------

Input Input::Instance;

// ----------------------------------------------------------------------------

Input::Input()
  : event_base(this)
{
}

// ----------------------------------------------------------------------------

Input::~Input()
{
}

// ----------------------------------------------------------------------------

void Input::Initialize()
{
  static Events::EventId updateId("update");
  SetHandler(updateId, &Input::OnUpdate);
}

// ----------------------------------------------------------------------------

void Input::OnKeyDown(const InputSignal& signal)
{

  KeyState *state = nullptr;
  if (signal.virtual_key < 0x100)
    state = &keyStates[signal.virtual_key];
  else
    state = &unicodeStates[signal.virtual_key];
  
  if (state->down)
    return;

  *state = signal;
  state->trigger_frame = GetGame()->Time.Frame;
  state->hold_time = 0;
  state->hold_time_buffer = 0;
  
  Events::EventId keyDownId("key_down");
  RaiseKeyEvent(keyDownId, *state);
  Events::EventId keyHeldId("key_held");
  RaiseKeyEvent(keyHeldId, *state);
}

// ----------------------------------------------------------------------------

void Input::OnKeyUp(const InputSignal& signal)
{
  KeyState *state = nullptr;
  if (signal.virtual_key < 0x100)
    state = &keyStates[signal.virtual_key];
  else
    state = &unicodeStates[signal.virtual_key];
  *state = signal;

  state->release_frame = GetGame()->Time.Frame;
  state->hold_time = 0;
  
  Events::EventId keyUpId("key_up");
  RaiseKeyEvent(keyUpId, *state);
}

// ----------------------------------------------------------------------------

const KeyState& Input::GetKeyState(virtual_key key)
{
  KeyState *state;
  if (key < 0x100)
    state = &keyStates[key];
  else
    state = &unicodeStates[key];

  if (!state->initialized)
  {
    state->virtual_key = key;
    state->char_code = VKeyToChar(key);
    state->initialized = true;
  }

  return *state;
}

// ----------------------------------------------------------------------------

wchar_t Input::VKeyToChar(virtual_key key)
{
  return (wchar_t) MapVirtualKey(key, MAPVK_VK_TO_CHAR);
}

// ----------------------------------------------------------------------------

InputSignal Input::TranslateSignal(UINT msg, WPARAM wParam, LPARAM)
{
  InputSignal signal;
  signal.virtual_key = (UINT) wParam;
  signal.char_code = VKeyToChar((UINT) wParam);
  signal.down = msg == WM_KEYDOWN;
  return signal;
}

// ----------------------------------------------------------------------------

void Input::OnUpdate(Events::EventMessage& e)
{
  auto dt = e.GetData<Events::UpdateEvent>()->gameTime.Dt;

  for (auto& state : keyStates)
  {
    UpdateState(state, dt);
  }

  for (auto& pair : unicodeStates)
  {
    UpdateState(pair.second, dt);
  }
}

// ----------------------------------------------------------------------------

void Input::UpdateState(KeyState& state, double dt)
{
  if (state.down)
  {
    state.hold_time += dt;
    state.hold_time_buffer += dt;

    if (state.hold_time_buffer >= key_hold_event_rate)
    {
      state.hold_time_buffer -= key_hold_event_rate;

      Events::EventId keyHeldId("key_held");
      RaiseKeyEvent(keyHeldId, state);
    }
  }
  else
  {
    state.hold_time = 0;
  }
}

// ----------------------------------------------------------------------------

void Input::RaiseKeyEvent(event_id id, KeyState& state)
{
  KeyStateEvent eventData{state};
  Events::EventMessage eventMsg{id, &eventData, true};
  Events::Event::Raise(eventMsg);
}

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

// ----------------------------------------------------------------------------

mrb_value KeyStateEvent::GetRubyWrapper()
{
  static mrb_state * const mrb = *mrb_inst;
  static mrb_sym new_sym = mrb_intern_cstr(mrb, "new");
  static RClass *rclass = nullptr;

  if (!rclass)
  {
    mrb_kse_data_type.dfree = mrb_kse_free;
    mrb_kse_data_type.struct_name = "const KeyState";

    rclass = mrb_define_class(mrb, "KeyState", mrb->object_class);

    // ruby can't make new ones
    mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_NONE());

    mrb_define_method(mrb, rclass, "held?", mrb_kse_is_held, ARGS_REQ(1));
    mrb_define_method(mrb, rclass, "hold_time", mrb_kse_hold_time, ARGS_REQ(1));
    mrb_define_method(mrb, rclass, "vkey", mrb_kse_vkey, ARGS_NONE());
    mrb_define_method(mrb, rclass, "plain_char?", mrb_kse_is_plain_char, ARGS_NONE());
    mrb_define_method(mrb, rclass, "plain_char", mrb_kse_plain_char, ARGS_NONE());

    #pragma region CONSTANTS

    DEF_INT_CONST("ESCAPE", VK_ESCAPE);
    DEF_INT_CONST("SPACE",  VK_SPACE);
    DEF_INT_CONST("ENTER",  VK_RETURN);
    DEF_INT_CONST("LEFT",   VK_LEFT);
    DEF_INT_CONST("RIGHT",  VK_RIGHT);
    DEF_INT_CONST("UP",     VK_UP);
    DEF_INT_CONST("DOWN",   VK_DOWN);

    #pragma endregion
  }

  return mrb_kse_new(mrb, state);
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
  auto key = (const KeyState *) mrb_data_get_ptr(mrb, self, &mrb_kse_data_type);
  return mrb_bool_value(key->down);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_hold_time(mrb_state *mrb, mrb_value self)
{
  auto key = (const KeyState *) mrb_data_get_ptr(mrb, self, &mrb_kse_data_type);
  return mrb_float_value(mrb, key->hold_time);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_vkey(mrb_state *mrb, mrb_value self)
{
  auto key = (const KeyState *) mrb_data_get_ptr(mrb, self, &mrb_kse_data_type);
  return mrb_fixnum_value(key->virtual_key);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_is_plain_char(mrb_state *mrb, mrb_value self)
{
  auto key = (const KeyState *) mrb_data_get_ptr(mrb, self, &mrb_kse_data_type);
  return mrb_bool_value(key->char_code >= 0x20 && key->char_code <= 0x7F);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_kse_plain_char(mrb_state *mrb, mrb_value self)
{
  auto key = (const KeyState *) mrb_data_get_ptr(mrb, self, &mrb_kse_data_type);
  if (key->char_code < 0x20 || key->char_code > 0x7F)
    return mrb_nil_value();

  char c[] = { (char) key->char_code, 0 };
  return mrb_str_new_cstr(mrb, c);
}

// ----------------------------------------------------------------------------


