/*********************************
 * Level.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#pragma once

class Level abstract
{
public:
  void init();
  void update();
  void draw();

protected:
  virtual void onUpdate() = 0;
};

