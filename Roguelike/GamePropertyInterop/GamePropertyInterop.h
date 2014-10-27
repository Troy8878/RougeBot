// GamePropertyInterop.h

#pragma once

#include "Stdafx.h"

using namespace System;

namespace GamePropertyInterop
{
  public ref class GameBase
  {
  public:
    static void RunGame();
  };

  public ref class EngineComponent
  {
  public:
    EngineComponent();
    EngineComponent(mrb_value component);
    ~EngineComponent();

    bool IsNil() { return mrb_nil_p(*component); };

  private:
    mrb_value *component;
  };
}
