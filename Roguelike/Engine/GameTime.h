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
  
  void update()
  {
    _prevTime = _currTime;
    _currTime = clock::now();

    auto timeDiff = _currTime - _prevTime;
    _dt = timeDiff.count() * clock::period::num / static_cast<double>(clock::period::den);

    if (_dt > maxDt)
      _dt = maxDt;
    if (_dt < minDt)
      _dt = minDt;
  }
  
  double dt() const
  {
    return _dt; 
  }

  clock::duration runningTime() const
  {
    return _currTime - _initTime;
  }

private:
  const clock::time_point _initTime = clock::now();
  clock::time_point _prevTime = clock::now();
  clock::time_point _currTime = clock::now();

  GameTime& operator=(const GameTime&) = delete;

  double _dt;
};
