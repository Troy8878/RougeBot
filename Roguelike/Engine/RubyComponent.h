/*********************************
 * RubyComponent.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

class RubyComponent : public Component
{
  
};

// ----------------------------------------------------------------------------

class RubyComponentFactory : public IComponentFactory
{
public:
  RubyComponentFactory(ruby::ruby_class rclass);
};

// ----------------------------------------------------------------------------


