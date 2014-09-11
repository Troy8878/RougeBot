/*********************************
 * Lq.h
 * Connor Hilarides
 * Created 2014/09/11
 *********************************/

#include <functional>

namespace lq
{
  template <typename Container>
  struct filter_range_wrapper
  {
    Container& container;
    std::function<bool(const Container::value_type&)> filter_func;

    filter_range_wrapper(Containter& container, const filter_func& func)
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

