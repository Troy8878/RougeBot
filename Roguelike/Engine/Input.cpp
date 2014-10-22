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

const MouseState& Input::GetMouseState()
{
  return mouse;
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