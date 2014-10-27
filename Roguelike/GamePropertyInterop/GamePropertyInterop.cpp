// This is the main DLL file.

#include "stdafx.h"

#include "GamePropertyInterop.h"

extern "C" void GameRunGame();

namespace GamePropertyInterop
{
  void GameBase::RunGame()
  {
    GameRunGame();
  }

  EngineComponent::EngineComponent()
    : EngineComponent(mrb_nil_value())
  {
  }

  EngineComponent::EngineComponent(mrb_value component)
    : component(new mrb_value(component))
  {
  }
  
  EngineComponent::~EngineComponent()
  {
    delete component;
  }
}
