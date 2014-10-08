/*********************************
* Input.cpp
* Connor Hilarides
* Created 2014/09/08
*********************************/

#include "Common.h"
#include "Input.h"
#include "RubyWrappers.h"

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

  auto& game = *GetGame();

  game.SetProcHandler(WM_KILLFOCUS, [this](HWND, UINT, WPARAM, LPARAM, LRESULT&)
  {
    AllKeysUp();
    AllMouseUp();
  });

  game.SetProcHandler(WM_KEYDOWN, [this](HWND, UINT msg, WPARAM wp, LPARAM lp, LRESULT&)
  {
    auto signal = TranslateSignal(msg, wp, lp);
    OnKeyDown(signal);
  });

  game.SetProcHandler(WM_KEYUP, [this](HWND, UINT msg, WPARAM wp, LPARAM lp, LRESULT&)
  {
    auto signal = TranslateSignal(msg, wp, lp);
    OnKeyUp(signal);
  });

  game.SetProcHandler(WM_MOUSEMOVE, [this](HWND, UINT, WPARAM, LPARAM lp, LRESULT&)
  {
    this->OnMouseMove(*reinterpret_cast<COORD *>(&lp));
  });

  static const virtual_key mouse_buttons[] =
  {
    MK_LBUTTON,
    MK_MBUTTON,
    MK_RBUTTON,
    MK_XBUTTON1,
    MK_XBUTTON2
  };

  game.SetProcHandlers([this](HWND, UINT, WPARAM wp, LPARAM lp, LRESULT&)
  {
    this->OnMouseMove(*reinterpret_cast<COORD *>(&lp));

    for (auto bt : mouse_buttons)
      if (wp & bt)
        this->OnMouseDown(bt);

  }, WM_LBUTTONDOWN, WM_MBUTTONDOWN, WM_RBUTTONDOWN, WM_XBUTTONDOWN);

  game.SetProcHandlers([this](HWND, UINT, WPARAM wp, LPARAM lp, LRESULT&)
  {
    this->OnMouseMove(*reinterpret_cast<COORD *>(&lp));

    for (auto bt : mouse_buttons)
      if (wp & bt)
        this->OnMouseUp(bt);

  }, WM_LBUTTONUP, WM_MBUTTONUP, WM_RBUTTONUP, WM_XBUTTONUP);

  game.SetProcHandlers([this](HWND, UINT, WPARAM wp, LPARAM lp, LRESULT&)
  {
    this->OnMouseMove(*reinterpret_cast<COORD *>(&lp));

    for (auto bt : mouse_buttons)
      if (wp & bt)
        this->OnDoubleClick(bt);

  }, WM_LBUTTONDBLCLK, WM_MBUTTONDBLCLK, WM_RBUTTONDBLCLK, WM_XBUTTONDBLCLK);
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
  
  DEF_EVENT_ID(key_down);
  RaiseKeyEvent(key_down, *state);
  DEF_EVENT_ID(key_held);
  RaiseKeyEvent(key_held, *state);
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
  
  DEF_EVENT_ID(key_up);
  RaiseKeyEvent(key_up, *state);
}

// ----------------------------------------------------------------------------

void Input::AllKeysUp()
{
  for (auto& state : keyStates)
  {
    if (state.down)
    {
      state.down = false;
      OnKeyUp(state);
    }
  }

  for (auto& pair : unicodeStates)
  {
    auto& state = pair.second;

    if (state.down)
    {
      state.down = false;
      OnKeyUp(state);
    }
  }
}

// ----------------------------------------------------------------------------

void Input::OnMouseMove(COORD position)
{
  mouse.position = math::Vector2D{(float)position.X, (float)position.Y};
  
  DEF_EVENT_ID(mouse_move);
  RaiseMouseEvent(mouse_move);
}

// ----------------------------------------------------------------------------

void Input::OnMouseDown(const virtual_key button)
{
  mouse.state |= button;
  mouse.lastButton = button;

  DEF_EVENT_ID(mouse_down);
  RaiseMouseEvent(mouse_down);
}

// ----------------------------------------------------------------------------

void Input::OnMouseUp(const virtual_key button)
{
  mouse.state &= ~button;
  mouse.lastButton = button;

  DEF_EVENT_ID(mouse_up);
  RaiseMouseEvent(mouse_up);
}

// ----------------------------------------------------------------------------

void Input::OnDoubleClick(const virtual_key button)
{
  mouse.lastButton = button;

  DEF_EVENT_ID(double_click);
  RaiseMouseEvent(double_click);
}

// ----------------------------------------------------------------------------

void Input::AllMouseUp()
{
  virtual_key const buttons[] =
  {
    VK_LBUTTON,
    VK_RBUTTON,
    VK_MBUTTON,
    VK_XBUTTON1,
    VK_XBUTTON2
  };

  for (const auto button : buttons)
  {
    if (mouse.ButtonDown(button))
      OnMouseUp(button);
  }
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

      DEF_EVENT_ID(key_held);
      RaiseKeyEvent(key_held, state);
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

void Input::RaiseMouseEvent(event_id id)
{
  MouseEvent data{&mouse};
  Events::EventMessage message{id, &data, true};
  Events::Event::Raise(message);
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

static mrb_data_type mrb_me_data_type;

static mrb_value mrb_me_new(mrb_state *mrb, const MouseState *state);
static void mrb_me_free(mrb_state *mrb, void *mem);

static mrb_value mrb_me_position(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_keystate_init(mrb_state *mrb)
{
  mrb_kse_data_type.dfree = mrb_kse_free;
  mrb_kse_data_type.struct_name = "const KeyState";

  RClass *rclass = mrb_define_class(mrb, "KeyState", mrb->object_class);

  // ruby can't make new ones
  mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_ANY());

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

  mrb_me_data_type.dfree = mrb_me_free;
  mrb_me_data_type.struct_name = "MouseState";

  rclass = mrb_define_class(mrb, "MouseState", mrb->object_class);

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
  const MouseState *state = (const MouseState *) 
    mrb_data_get_ptr(mrb, self, &mrb_me_data_type);

  return ruby::create_new_vector(state->position.get());
}

// ----------------------------------------------------------------------------


