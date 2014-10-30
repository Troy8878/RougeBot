/*********************************
 * PropertyServer.h
 * Connor Hilarides
 * Created 2014/10/27
 *********************************/

#pragma once

#include "Engine/Common.h"

// ----------------------------------------------------------------------------

struct PropertyServerInternal;

// ----------------------------------------------------------------------------

class PropertyServer
{
public:
  PropertyServer();
  ~PropertyServer();

  NO_ASSIGNMENT_OPERATOR(PropertyServer);
  NO_COPY_CONSTRUCTOR(PropertyServer);

private:
  PropertyServerInternal * data;
};

// ----------------------------------------------------------------------------
