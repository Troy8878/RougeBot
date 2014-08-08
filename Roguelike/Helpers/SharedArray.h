/*********************************
 * SharedArray.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#pragma once

#include <memory>

template <typename T>
class shared_array
{
  struct array_holder
  {
    T *data = nullptr;

    array_holder() = default;
    array_holder(size_t size) : data(new byte[size]) { }
    ~array_holder() { delete[] data; }
  };

public:
  shared_array() = default;
  shared_array(nullptr_t) {};
  explicit shared_array(size_t size) : ptr(std::make_shared<array_holder>(size)) { }

  operator T *() { return ptr->data; }
  T *get() { return ptr->data; }

private:
  std::shared_ptr<array_holder> ptr;
};
