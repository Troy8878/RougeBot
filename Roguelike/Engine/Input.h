/*********************************
 * Input.h
 * Connor Hilarides
 * Created 2014/09/04
 *********************************/

#include "Engine/Common.h"

// ----------------------------------------------------------------------------

typedef wchar_t virtual_key;

// ----------------------------------------------------------------------------

struct InputSignal
{
  virtual_key virtual_key;
  wchar_t char_code;
  bool down;
};

struct KeyState : InputSignal
{
  double hold_time;

  KeyState& operator=(const InputSignal& in) 
  { 
    InputSignal::operator=(in); 
    return *this; 
  }
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


