/*********************************
 * Exceptions.h
 * Connor Hilarides
 * Created 2014/07/01
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include <exception>
#include "FixedWindows.h"

namespace std
{

  class not_implemented_exception : public exception
  {
    std::string _message = "This has not yet been implemented, or never will";

  public:
    not_implemented_exception() = default;
    not_implemented_exception(const std::string& message)
      : _message(message)
    {
    }

    const char *what() const override
    {
      return _message.c_str();
    }
  };

}

