/*********************************
 * RubyWrappers.h
 * Connor Hilarides
 * Created 2014/08/29
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

// ----------------------------------------------------------------------------

namespace ruby
{

// ----------------------------------------------------------------------------

  mrb_value wrap_memory_vector(math::Vector *vect);
  mrb_value wrap_memory_vector(math::Vector2D *vect);

// ----------------------------------------------------------------------------

  extern mrb_data_type mrb_vector_type;

  mrb_value create_new_vector(const math::Vector& v = {});
  math::Vector& get_ruby_vector(mrb_value value);

// ----------------------------------------------------------------------------

}
