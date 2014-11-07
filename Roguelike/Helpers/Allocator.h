/*********************************
 * Allocator.h
 * Connor Hilarides
 * Created 2014/08/20
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

// ----------------------------------------------------------------------------

__interface IAllocator
{
public:
  void *Allocate();
  void Free(void *);
};

// ----------------------------------------------------------------------------
