/*********************************
 * VisualizePerformance.h
 * Connor Hilarides
 * Created 2014/09/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include <chrono>
#include <string>

#include "mruby.h"

namespace performance
{
  typedef std::chrono::high_resolution_clock clock;

#ifdef _DEBUG

  void register_diagnostic(clock::duration duration, std::string&& identifier);

  class register_guard
  {
  public:
    inline register_guard(const std::string& identifier)
      : register_guard(std::string{identifier}) {}

    register_guard(std::string&& identifier);
    register_guard(mrb_value object, mrb_sym method);
    ~register_guard();

  private:
    clock::time_point start;
    std::string identifier;
  };

#else

  inline void register_diagnostic(clock::duration, std::string&&) {}

  class register_guard
  {
  public:
    inline register_guard(const std::string&) {}
    inline register_guard(std::string&&) {}
    inline register_guard(mrb_value, mrb_sym) {}
    inline ~register_guard() {}
  };

#endif

}
