/*********************************
 * Button.h
 * Leonardo Saikali
 * Created 2014/10/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Engine/Common.h"

// ----------------------------------------------------------------------------

class ButtonManager : public Events::BasicClassEventReciever<ButtonManager>
{
public:
  static ButtonManager Instance;

  ButtonManager();
  void Initialize();

  void OnClick(Events::EventMessage &e);
  void OnMove(Events::EventMessage &e);

private:
  std::shared_ptr<Entity *> lastMatch = nullptr;
};

// ----------------------------------------------------------------------------

struct ButtonProbeEvent : public Events::EventData
{
  Entity *bestMatch = nullptr;
  float matchDistance = -1;
};

// ----------------------------------------------------------------------------