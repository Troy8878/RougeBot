/*********************************
 * jquery.h
 * Connor Hilarides
 * Created 2015/03/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once
#include <Helpers/RandomHelpers.h>
#include "json.h"
#include <functional>

namespace json
{
  class query_path;

  query_path query(value val);

  class query_path
  {
  public:
    query_path at(const std::string &str);
    query_path at(size_t index);

    void array_map(const std::function<void(value)> &func);
    void object_map(const std::function<void(const std::string &, value)> &func);
    template <typename T>
    std::vector<T> array_map(const std::function<T(value)> &func);
    template <typename T>
    std::vector<T> object_map(const std::function<T(const std::string &, value)> &func);

    option_ref<value::object_t> is_object();
    option_ref<value::array_t> is_array();
    option_ref<value::string_t> is_string();
    option<value::number_t> is_number();
    option<value::bool_t> is_bool();

  private:
    query_path();
    query_path(value val);
    friend query_path query(value val);

    option<value> object;
  };
}

template <typename T>
std::vector<T> json::query_path::array_map(const std::function<T(value)> &func)
{
  std::vector<T> result;
  array_map([&result, &func](value object)
  {
    result.push_back(func(object));
  });
  return result;
}

template <typename T>
std::vector<T> json::query_path::object_map(const std::function<T(const std::string &, value)> &func)
{
  std::vector<T> result;
  object_map([&result, &func](const std::string &key, value object)
  {
    result.push_back(func(key, object));
  });
  return result;
}
