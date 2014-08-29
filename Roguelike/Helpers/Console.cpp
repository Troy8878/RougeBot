/*********************************
 * Console.cpp
 * Connor Hilarides
 * Created 2014/07/09
 *********************************/

#include "Console.h"

std::ostream& debug = std::cout;
console::fg console::current_fg = fg::white;
console::bg console::current_bg = bg::black;
