/*********************************
 * CriticalSections.cpp
 * Connor Hilarides
 * Created 2014/08/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "CriticalSections.h"

critical_section::critical_section()
{
  InitializeCriticalSectionEx(&_section, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
}

critical_section::~critical_section()
{
  DeleteCriticalSection(&_section);
}

void critical_section::enter()
{
  EnterCriticalSection(&_section);
}

void critical_section::leave()
{
  LeaveCriticalSection(&_section);
}

CRITICAL_SECTION *critical_section::handle()
{
  return &_section;
}

critical_section::guard::guard(critical_section& crit)
  : crit(crit)
{
  crit.enter();
}

critical_section::guard::~guard()
{
  crit.leave();
}

