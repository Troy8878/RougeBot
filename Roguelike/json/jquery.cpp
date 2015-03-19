/*********************************
 * jquery.cpp
 * Connor Hilarides
 * Created 2015/03/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "jquery.h"

json::query_path json::query(value val)
{
  return query_path(val);
}

json::query_path json::query_path::at(const std::string &str)
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jobject))
    {
      auto &obj = val.as_object();
      auto it = obj.find(str);
      if (it != obj.end())
      {
        return query_path(it->second);
      }
    }
  }

  return {};
}

json::query_path json::query_path::at(size_t index)
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jarray))
    {
      auto &ary = val.as_array();
      if (ary.size() > index)
      {
        return query_path(ary[index]);
      }
    }
  }

  return {};
}

void json::query_path::array_map(const std::function<void(value)> &func)
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jarray))
    {
      for (auto &item : val.as_array())
      {
        func(item);
      }
    }
  }
}

void json::query_path::object_map(const std::function<void(const std::string &, value)> &func)
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jobject))
    {
      for (auto &item : val.as_object())
      {
        func(item.first, item.second);
      }
    }
  }
}

option_ref<json::value::object_t> json::query_path::is_object()
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jobject))
    {
      return val.as_object();
    }
  }

  return {};
}

option_ref<json::value::array_t> json::query_path::is_array()
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jarray))
    {
      return val.as_array();
    }
  }

  return {};
}

option_ref<json::value::string_t> json::query_path::is_string()
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jstring))
    {
      return val.as_string();
    }
  }

  return {};
}

option<json::value::number_t> json::query_path::is_number()
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jnumber))
    {
      return val.as_number();
    }
  }

  return {};
}

option<json::value::bool_t> json::query_path::is_bool()
{
  if (object.is_set())
  {
    auto &val = object.get();
    if (val.is(json_type::jbool))
    {
      return val.as_bool();
    }
  }

  return {};
}

json::query_path::query_path()
  : object()
{
}

json::query_path::query_path(value val)
  : object(std::move(val))
{
}
