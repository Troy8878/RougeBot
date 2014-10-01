/*********************************
 * regexp.h
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#pragma once

#include "mruby.h"

#ifdef __cplusplus
#include <regex>

std::regex& mrb_regexp_cppregex(mrb_state *mrb, mrb_value regexp);
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------

extern mrb_data_type mrb_regex_data;

mrb_value mrb_regexp_split(mrb_state *mrb, mrb_value str_v, mrb_value regexp, mrb_int lim);

// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
