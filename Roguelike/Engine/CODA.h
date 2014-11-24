/*********************************
 * CODA.h
 * Connor Hilarides
 * Created 2014/11/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Helpers\FixedWindows.h"

class ConfirmationOfDestructiveAction
{
public:

private:
  LRESULT PatchedWndProc(HWND, UINT msg, WPARAM wp, LPARAM lp);
};
