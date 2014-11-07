/*********************************
 * Lq.h
 * Connor Hilarides
 * Created 2014/09/11
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include <functional>

namespace lq
{
  template <typename Container>
  struct filter_range_wrapper
  {
    Container& container;
    typedef std::function<bool(const typename Container::value_type&)> filter_func;
    filter_func func;

    filter_range_wrapper(Container& container, const filter_func& func)
      : container(container), func(func)
    {
    }

    struct iterator
    {
    };

    iterator begin()
    {
    }

    iterator end()
    {
    }
  };
}

