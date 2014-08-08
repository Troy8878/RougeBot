/*********************************
 * CriticalSections.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "FixedWindows.h"

#define THREAD_EXCLUSIVE_SCOPE \
  static critical_section ___critSec___; \
  critical_section::guard factory_guard{___critSec___}

class critical_section
{
public:
  critical_section();
  ~critical_section();

  void enter();
  void leave();

  class guard
  {
  public:
    guard(critical_section& crit);
    ~guard();

  private:
    critical_section& crit;
  };

private:
  CRITICAL_SECTION _section;
};

