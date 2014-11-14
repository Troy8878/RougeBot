/*********************************
 * Console.cpp
 * Connor Hilarides
 * Created 2014/07/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Console.h"

std::ostream& debug = std::cout;
console::fg console::current_fg = fg::white;
console::bg console::current_bg = bg::black;
