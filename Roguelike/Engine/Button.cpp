/*********************************
 * Button.cpp
 * Leonardo Saikali
 * Created 2014/10/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Button.h"

// ----------------------------------------------------------------------------

ButtonManager ButtonManager::Instance;

// ----------------------------------------------------------------------------

ButtonManager::ButtonManager()
  : event_base(this)
{
}

// ----------------------------------------------------------------------------

void ButtonManager::OnClick(Events::EventMessage &)
{
  DEF_EVENT_ID(button_probe);

  ButtonProbeEvent data;
  Events::EventMessage message{button_probe, &data, false};

  Events::Event::Raise(message);

  if (data.bestMatch)
  {
    DEF_EVENT_ID(button_clicked);

    Events::EventMessage message{button_clicked, nullptr};

    data.bestMatch->RaiseEvent(message);
  }
}

// ----------------------------------------------------------------------------