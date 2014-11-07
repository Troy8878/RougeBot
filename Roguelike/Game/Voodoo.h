/*********************************
 * Voodoo.h
 * Connor Hilarides
 * Created 2014/09/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include <string>

std::string translate_method_name(void *code_addr);

template <typename R, typename... A>
std::string translate_method_name(R (*code_addr)(A...))
{
  return translate_method_name((void *) code_addr);
}

template <typename C, typename R, typename... A>
std::string translate_method_name(R (C::*code_addr)(A...))
{
  union
  {
    typedef decltype(code_addr) mfp_t;
    mfp_t mfp;
    void *vp;
  } func;

  func.mfp = code_addr;
  
  return translate_method_name(func.vp);
}



