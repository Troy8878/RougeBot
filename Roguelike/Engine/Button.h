/*********************************
* Button.h
* Leonardo Saikali
* Created 2014/10/07
*********************************/

#pragma once

#include "Engine/Common.h"

// ----------------------------------------------------------------------------

class ButtonManager : public Events::BasicClassEventReciever<ButtonManager>
{
public:
  static ButtonManager Instance;

  ButtonManager();

  void OnClick(Events::EventMessage &e);
};

// ----------------------------------------------------------------------------

struct ButtonProbeEvent : public Events::EventData
{
  Entity *bestMatch = nullptr;
  float matchDistance = -1;
};

// ----------------------------------------------------------------------------