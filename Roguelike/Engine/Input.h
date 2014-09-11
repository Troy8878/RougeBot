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
//      Is triggered every 0.5 seconds while a key is held down
//      type: KeyStateEvent

// ----------------------------------------------------------------------------

typedef wchar_t virtual_key;

// ----------------------------------------------------------------------------

struct InputSignal
{
  virtual_key virtual_key = 0;
  wchar_t char_code = 0;
  bool down = false;
};

struct KeyState : InputSignal
{
  double hold_time = 0;
  bool triggered = false;
  bool released = false;

  KeyState& operator=(const InputSignal& in)
  { 
    InputSignal::operator=(in); 
    return *this; 
  }

private:
  double hold_time_buffer = 0;
  bool initialized = false;

  friend class Input;
};

// ----------------------------------------------------------------------------

class Input : public Events::BasicClassEventReciever<Input>
{
public:
  Input();
  ~Input();

  // Called when the game detects a keydown signal
  void OnKeyDown(const InputSignal& signal);
  // Called when the game detects a keyup signal
  void OnKeyUp(const InputSignal& signal);

  // Gets the current state of the given virtual key
  const KeyState& GetKeyState(virtual_key key);

  // Returns the character for a given virtual key,
  // taking into account CAPSLOCK and SHIFT modifiers.
  static wchar_t TranslateVKey(virtual_key key);

  // Given the Win32 message parameters, create a new
  // InputSignal with the proper fields set.
  static InputSignal TranslateSignal(UINT msg, WPARAM wParam, LPARAM lParam);

private:
  // Handler for the update event
  void OnUpdate(Events::EventMessage& e);

  KeyState keyStates[0x100];
  std::unordered_map<virtual_key, KeyState> unicode_states;
  // add in the other variables you need to keep track of the input state
  // feel free to modify KeyState if you feel like that's the best way
  // Remember to check for unicode values on VK_PACKET
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
};

// ----------------------------------------------------------------------------


