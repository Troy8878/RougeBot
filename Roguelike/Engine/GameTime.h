/*********************************
 * GameTime.h
 * Connor Hilarides
 * Created 2014/07/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

class GameTime
{
public:
  typedef std::chrono::high_resolution_clock clock;
  const double maxDt = 0.1;
  const double minDt = 0;
  const double clock_period = clock::period::num / static_cast<double>(clock::period::den);

  // ----------------------------------------------------------------------------

  GameTime()
    : _Paused(false)
  {
  }

  // ----------------------------------------------------------------------------

  IR_PROPERTY(double, Dt);
  IR_PROPERTY(uint64_t, Frame);
  IR_PROPERTY(clock::duration, TimeDiff);
  IR_PROPERTY(bool, Paused);
  PROPERTY(get = _GetRunningTime) clock::duration RunningTime;
  PROPERTY(get = _GetCurrFrameTime) double CurrFrameTime;

  // ----------------------------------------------------------------------------

  void Update()
  {
    ++Frame;

    _prevTime = _currTime;
    _currTime = clock::now();

    TimeDiff = _currTime - _prevTime;
    Dt = TimeDiff.count() * clock_period;

    if (Dt > maxDt)
      Dt = maxDt;
    if (Dt < minDt)
      Dt = maxDt;

    if (Paused)
      Dt = 0;
  }

  // ----------------------------------------------------------------------------

  clock::duration _GetRunningTime() const
  {
    return _currTime - _initTime;
  }

  // ----------------------------------------------------------------------------

  double _GetCurrFrameTime() const
  {
    auto time = clock::now();
    auto diff = time - _currTime;

    return diff.count() * clock_period;
  }

  // ----------------------------------------------------------------------------

private:
  const clock::time_point _initTime = clock::now();
  clock::time_point _prevTime = clock::now();
  clock::time_point _currTime = clock::now();

  GameTime &operator=(const GameTime &) = delete;
};

// ----------------------------------------------------------------------------