/*********************************
 * GameTime.h
 * Connor Hilarides
 * Created 2014/07/07
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
  
  void Update()
  {
    _prevTime = _currTime;
    _currTime = clock::now();

    auto timeDiff = _currTime - _prevTime;
    Dt = timeDiff.count() * clock_period;

    if (Dt > maxDt)
      Dt = maxDt;
    if (Dt < minDt)
      Dt = maxDt;
  }

// ----------------------------------------------------------------------------
  
  IR_PROPERTY(double, Dt);

// ----------------------------------------------------------------------------

  clock::duration RunningTime() const
  {
    return _currTime - _initTime;
  }

// ----------------------------------------------------------------------------

  __declspec(property(get = _GetCurrFrameTime)) double CurrFrameTime;
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

  GameTime& operator=(const GameTime&) = delete;
};

// ----------------------------------------------------------------------------
