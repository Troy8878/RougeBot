/*********************************
 * Allocator.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

// ----------------------------------------------------------------------------

class Allocator abstract
{
public:
  virtual void *Allocate() = 0;
  virtual void Free(void *) = 0;

protected:
  inline ~Allocator() {}
};

// ----------------------------------------------------------------------------
