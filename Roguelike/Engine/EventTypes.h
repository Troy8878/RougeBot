/*********************************
 * EventTypes.h
 * Connor Hilarides
 * Created 2014/08/12
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
    virtual mrb_value GetRubyWrapper()
    { 
      return ruby::ruby_value{}; 
    };
  };

// ----------------------------------------------------------------------------

  template <typename T>
  struct RudimentaryEventWrapper : public EventData
  {
    T data;

    RudimentaryEventWrapper() = default;
    RudimentaryEventWrapper(const T& data)
      : data(data)
    {
    }

    NO_ASSIGNMENT_OPERATOR(RudimentaryEventWrapper<T>);
  };

// ----------------------------------------------------------------------------

  struct UpdateEvent : public EventData
  {
    GameTime& gameTime;

    UpdateEvent(GameTime& gameTime)
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

}

