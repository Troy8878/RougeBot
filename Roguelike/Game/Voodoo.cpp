/*********************************
 * Voodoo.cpp
 * Connor Hilarides
 * Created 2014/09/21
 *********************************/

#include "Common.h"
#include "Voodoo.h"

std::string translate_method_name(void *code_addr)
{
  auto trace = stack_trace::get_trace_line(code_addr);
  return trace.method;
}
