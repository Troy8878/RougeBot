/*********************************
 * EventTypes.h
 * Connor Hilarides
 * Created 2014/08/12
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

// ----------------------------------------------------------------------------

class GameTime;

namespace Events
{
  // ----------------------------------------------------------------------------

  struct EventData
  {
  protected:
    virtual ~EventData()
    {
    }

  public:
    virtual mrb_value GetRubyWrapper()
    {
      return mrb_nil_value();
    };
  };

  // ----------------------------------------------------------------------------

  template <typename T>
  struct RudimentaryEventWrapper : public EventData
  {
    T data;

    RudimentaryEventWrapper() = default;

    explicit RudimentaryEventWrapper(const T &data)
      : data(data)
    {
    }

    NO_ASSIGNMENT_OPERATOR(RudimentaryEventWrapper<T>);
  };

  // ----------------------------------------------------------------------------

  struct UpdateEvent : public EventData
  {
    GameTime &gameTime;

    UpdateEvent(GameTime &gameTime)
      : gameTime(gameTime)
    {
    }

    mrb_value GetRubyWrapper() override;

    NO_ASSIGNMENT_OPERATOR(UpdateEvent);
  };

  // ----------------------------------------------------------------------------

  struct EventRecieverDestroyedEvent : public EventData
  {
    EventReciever *reciever;

    EventRecieverDestroyedEvent(EventReciever *reciever)
      : reciever(reciever)
    {
    }
  };

  // ----------------------------------------------------------------------------

  struct RubyEvent : public EventData
  {
    mrb_value ruby_obj;

    explicit RubyEvent(mrb_value ruby_obj)
      : ruby_obj(ruby_obj)
    {
    }

    mrb_value GetRubyWrapper() override
    {
      return ruby_obj;
    }
  };

  // ----------------------------------------------------------------------------
}