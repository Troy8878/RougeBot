/*********************************
 * Input.h
 * Connor Hilarides
 * Created 2014/09/04
 *********************************/

#pragma once

#include "Engine/Common.h"

// ----------------------------------------------------------------------------

//  Events
//    key_triggered
//      Happens once when a key is pushed down
//      type: KeyStateEvent
//
//    key_released
//      Happens once when a key is released
//      type: KeyStateEvent
//
//    key_held
//      Is triggered every [key_hold_event_rate] seconds
//      while a key is held down 
//      type: KeyStateEvent
//
//    mouse_move
//      Whenever a mouse moves, we'll be there
//      type: MouseEvent
//    
//    mouse_down
//      Whenever a mouse button is pressed
//      type: MouseEvent
//
//    mouse_up
//      Whenever a mouse button is released
//      type: MouseEvent
//
//    double_click
//      Whenever a mouse button is double-clicked
//      type: MouseEvent

// ----------------------------------------------------------------------------

typedef UINT virtual_key;

// ----------------------------------------------------------------------------

const double key_hold_event_rate = 0.1;

struct InputSignal
{
  virtual_key virtual_key = 0;
  wchar_t char_code = 0;
  bool down = false;
};

// ----------------------------------------------------------------------------

#pragma region States

struct KeyState : InputSignal
{
  double hold_time = 0;
  uint64_t trigger_frame = 0;
  uint64_t release_frame = 0;

  PROPERTY(get = _GetTriggered) bool triggered;
  PROPERTY(get = _GetReleased) bool released;

  KeyState& operator=(const InputSignal& in)
  { 
    InputSignal::operator=(in); 
    return *this; 
  }

private:
  double hold_time_buffer = 0;
  bool initialized = false;

  friend class Input;

public:
  bool _GetTriggered() { return trigger_frame == GetGame()->Time.Frame; };
  bool _GetReleased() { return release_frame == GetGame()->Time.Frame; };
};

// ----------------------------------------------------------------------------

struct MouseState
{
  math::Vector2D position;
  double scrollPosition;
  UINT state;
  UINT lastButton;

  bool ButtonDown(UINT button) { return !!(state & button); }
  bool ButtonUp(UINT button) { return !(state & button); }
};

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Event Data

struct KeyStateEvent : public Events::EventData
{
  const KeyState *state;

  KeyStateEvent(const KeyState& state)
    : state(&state)
  {
  }

  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct MouseEvent : public Events::EventData
{
  const MouseState *state;

  MouseEvent(const MouseState *state)
    : state(state)
  {
  }
  
  mrb_value GetRubyWrapper() override;
};

#pragma endregion

// ----------------------------------------------------------------------------

class Input : public Events::BasicClassEventReciever<Input>
{
public:
  Input();
  ~Input();

  static Input Instance;

  void Initialize();

  // Called when the game detects a keydown signal
  void OnKeyDown(const InputSignal& signal);
  // Called when the game detects a keyup signal
  void OnKeyUp(const InputSignal& signal);
  void AllKeysUp();

  void OnMouseMove(COORD position);

  void OnMouseDown(const virtual_key button);
  void OnMouseUp(const virtual_key button);
  void OnDoubleClick(const virtual_key button);
  void AllMouseUp();

  // Gets the current state of the given virtual key
  const KeyState& GetKeyState(virtual_key key);

  // Given the Win32 message parameters, create a new
  // InputSignal with the proper fields set.
  static InputSignal TranslateSignal(UINT msg, WPARAM wParam, LPARAM lParam);

private:
  // Handler for the update event
  void OnUpdate(Events::EventMessage& e);
  void UpdateState(KeyState& state, double dt);
  void RaiseKeyEvent(event_id id, KeyState& state);
  void RaiseMouseEvent(event_id id);

  static wchar_t VKeyToChar(virtual_key key);

  MouseState mouse;
  KeyState keyStates[0x100];
  std::unordered_map<virtual_key, KeyState> unicodeStates;
};

// ----------------------------------------------------------------------------


