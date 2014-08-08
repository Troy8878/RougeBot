/*********************************
 * GameTime.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#pragma once

#include "Common.h"

class GameTime
{
public:
  typedef std::chrono::high_resolution_clock clock;
  const double maxDt = 0.1;
  const double minDt = 0;
  const double clock_period = clock::period::num / static_cast<double>(clock::period::den);
  
  void update()
  {
    _prevTime = _currTime;
    _currTime = clock::now();

    auto timeDiff = _currTime - _prevTime;
    _dt = timeDiff.count() * clock_period;

    if (_dt > maxDt)
      _dt = maxDt;
    if (_dt < minDt)
      _dt = maxDt;
  }
  
  double dt() const
  {
    return _dt; 
  }

  clock::duration runningTime() const
  {
    return _currTime - _initTime;
  }

  double currFrameTime() const
  {
    auto time = clock::now();
    auto diff = time - _currTime;
    
    return diff.count() * clock_period;
  }

private:
  const clock::time_point _initTime = clock::now();
  clock::time_point _prevTime = clock::now();
  clock::time_point _currTime = clock::now();

  GameTime& operator=(const GameTime&) = delete;

  double _dt;
};
