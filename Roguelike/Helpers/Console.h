/*********************************
 * Console.h
 * Connor Hilarides
 * Created 2014/05/30
 *********************************/

#pragma once

#include "FixedWindows.h"
#include <iostream>

class console
{
public:
  enum class fg : WORD
  {
    black             = 0,
    darkblue          = FOREGROUND_BLUE,
    darkgreen         = FOREGROUND_GREEN,
    darkcyan          = FOREGROUND_GREEN | FOREGROUND_BLUE,
    darkred           = FOREGROUND_RED,
    darkmagenta       = FOREGROUND_RED | FOREGROUND_BLUE,
    darkyellow        = FOREGROUND_RED | FOREGROUND_GREEN,
    darkgray          = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    gray              = FOREGROUND_INTENSITY,
    blue              = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    green             = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    cyan              = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    red               = FOREGROUND_INTENSITY | FOREGROUND_RED,
    magenta           = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    yellow            = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    white             = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
  };

  enum class bg : WORD
  {
    black             = 0,
    darkblue          = BACKGROUND_BLUE,
    darkgreen         = BACKGROUND_GREEN,
    darkcyan          = BACKGROUND_GREEN | BACKGROUND_BLUE,
    darkred           = BACKGROUND_RED,
    darkmagenta       = BACKGROUND_RED | BACKGROUND_BLUE,
    darkyellow        = BACKGROUND_RED | BACKGROUND_GREEN,
    darkgray          = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    gray              = BACKGROUND_INTENSITY,
    blue              = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
    green             = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
    cyan              = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
    red               = BACKGROUND_INTENSITY | BACKGROUND_RED,
    magenta           = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
    yellow            = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
    white             = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
  };

private:
  static fg current_fg;
  static bg current_bg;

  static void update()
  {
    static auto console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, (WORD)current_fg | (WORD)current_bg);
  }

public:
  static void fg_color(fg fgcolor)
  {
    current_fg = fgcolor;
    update();
  }

  static void bg_color(bg bgcolor)
  {
    current_bg = bgcolor;
    update();
  }

  static fg fg_color() { return current_fg; }
  static bg bg_color() { return current_bg; }
};

template <typename _Elem, typename _Trait = char_traits<_Elem>>
inline std::basic_ostream<_Elem, _Trait>& operator<<
  (std::basic_ostream<_Elem, _Trait>& os, console::fg fgcolor)
{
  console::fg_color(fgcolor);
  return os;
}

template <typename _Elem, typename _Trait = char_traits<_Elem>>
inline std::basic_ostream<_Elem, _Trait>& operator<<
  (std::basic_ostream<_Elem, _Trait>& os, console::bg bgcolor)
{
  console::bg_color(bgcolor);
  return os;
}

extern std::ostream& debug;
