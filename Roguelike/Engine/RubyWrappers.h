/*********************************
 * RubyWrappers.h
 * Connor Hilarides, Enrique Rodriguez, Leonardo Saikali
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
  mrb_value wrap_memory_matrix(math::Matrix *mat);

  // ----------------------------------------------------------------------------

  extern mrb_data_type mrb_vector_type;
  extern mrb_data_type mrb_mvector_type;

  mrb_value create_new_vector(const math::Vector &v = {});
  math::Vector &get_ruby_vector(mrb_value value);

  // ----------------------------------------------------------------------------

  extern mrb_data_type mrb_matrix_type;
  extern mrb_data_type mrb_mmatrix_type;

  mrb_value create_new_matrix(const math::Matrix &m);
  math::Matrix &get_ruby_matrix(mrb_value value);

}