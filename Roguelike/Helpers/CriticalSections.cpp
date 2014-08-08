/*********************************
 * CriticalSections.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "CriticalSections.h"

critical_section::critical_section()
{
  InitializeCriticalSection(&_section);
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

critical_section::guard::guard(critical_section& crit)
  : crit(crit)
{
  crit.enter();
}

critical_section::guard::~guard()
{
  crit.leave();
}

