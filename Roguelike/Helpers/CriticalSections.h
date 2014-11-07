/*********************************
 * CriticalSections.h
 * Connor Hilarides
 * Created 2014/08/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "FixedWindows.h"

#define THREAD_EXCLUSIVE_SCOPE \
  static critical_section ___critSec___; \
  critical_section::guard ___critSecGuard___{___critSec___}

class critical_section
{
public:
  critical_section();
  ~critical_section();

  void enter();
  void leave();

  CRITICAL_SECTION *handle();

  class guard
  {
  public:
    guard(critical_section& crit);
    ~guard();

    guard& operator=(const guard&) = delete;

  private:
    critical_section& crit;
  };

private:
  CRITICAL_SECTION _section;
};

