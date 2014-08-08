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
    size_t size;
    T *data = nullptr;

    array_holder() = default;
    array_holder(size_t size) : size(size), data(new byte[size]) { }
    ~array_holder() { delete[] data; }
  };

public:
  shared_array() = default;
  shared_array(nullptr_t) {};
  explicit shared_array(size_t size) : ptr(std::make_shared<array_holder>(size)) { }
  shared_array(std::initializer_list<T> items)
    : shared_array(items.size())
  {
    T *data = get();
    for (auto& item : items)
      *data++ = item;
  }
  template <size_t size>
  shared_array(T (&items)[size])
    : shared_array(size)
  {
    T *data = get();
    for (auto& item : items)
      *data++ = item;
  }

  operator T *() { return ptr->data; }
  T *get() { return ptr->data; }
  T& operator[](size_t index) { return ptr->data[index]; }

  size_t size() { return ptr->size; }

private:
  std::shared_ptr<array_holder> ptr;
};
