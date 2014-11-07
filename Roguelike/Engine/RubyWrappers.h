/*********************************
 * RubyWrappers.h
 * Connor Hilarides
 * Created 2014/08/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

// ----------------------------------------------------------------------------

namespace ruby
{
  // ----------------------------------------------------------------------------

  mrb_value wrap_memory_vector(math::Vector *vect);

  // ----------------------------------------------------------------------------

  extern mrb_data_type mrb_vector_type;
  extern mrb_data_type mrb_mvector_type;

  mrb_value create_new_vector(const math::Vector &v = {});
  math::Vector &get_ruby_vector(mrb_value value);

  // ----------------------------------------------------------------------------
}