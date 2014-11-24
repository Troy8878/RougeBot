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

void ButtonManager::Initialize()
{
  DEF_EVENT_ID(mouse_down);
  SetHandler(mouse_down, &ButtonManager::OnClick);

  DEF_EVENT_ID(update);
  SetHandler(update, &ButtonManager::OnMove);
}

// ----------------------------------------------------------------------------

void ButtonManager::OnClick(Events::EventMessage &e)
{
  DEF_EVENT_ID(button_probe);

  ButtonProbeEvent data;
  Events::EventMessage message{button_probe, &data, false};

  Events::Event::Raise(message);

  if (data.bestMatch)
  {
    e.Handled = true;

    DEF_EVENT_ID(button_clicked);
    Events::EventMessage message{button_clicked, nullptr};

    data.bestMatch->RaiseEvent(message);
  }
}

// ----------------------------------------------------------------------------

void ButtonManager::OnMove(Events::EventMessage &)
{
  DEF_EVENT_ID(button_probe);

  ButtonProbeEvent data;
  Events::EventMessage message{button_probe, &data, false};

  Events::Event::Raise(message);

  if (data.bestMatch)
  {
    if (data.bestMatch->SelfRef != lastMatch)
    {
      if (lastMatch && *lastMatch)
      {
        DEF_EVENT_ID(mouse_leave);
        Events::EventMessage message{mouse_leave, nullptr};
        (*lastMatch)->RaiseEvent(message);
      }

      lastMatch = data.bestMatch->SelfRef;

      {
        DEF_EVENT_ID(mouse_enter);
        Events::EventMessage message{mouse_enter, nullptr};
        (*lastMatch)->RaiseEvent(message);
      }
    }
  }
  else
  {
    if (lastMatch && *lastMatch)
    {
      DEF_EVENT_ID(mouse_leave);
      Events::EventMessage message{mouse_leave, nullptr};
      (*lastMatch)->RaiseEvent(message);
    }

    lastMatch = nullptr;
  }
}

// ----------------------------------------------------------------------------