/*********************************
 * FlatMap.h
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#pragma once

#include <vector>
#include <utility>
#include <algorithm>

template <typename _Key, typename _Value, typename Comparator = std::less<_Key>>
class flat_map
{
public:
  typedef _Key key_type;
  typedef _Value value_type;

private:
  typedef std::pair<key_type, value_type> pair_type;
  typedef std::vector<pair_type> container_type;

  template <typename Comp>
  struct pair_comparator
  {
    bool operator()(const pair_type& a, const key_type& b)
    {
      Comp comp;
      return comp(a.first, b);
    }

    bool operator()(const pair_type& a, const pair_type& b)
    {
      Comp comp;
      return comp(a.first, b.first);
    }
  };

  typedef pair_comparator<Comparator> comparator_type;

public:
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

  iterator find(const key_type& key)
  {
    comparator_type comparator;
    return std::lower_bound(begin(), end(), key, comparator);
  }

  value_type& operator[](const key_type& key)
  {
    auto it = find(key);
    if (it == end())
    {
      insert(key, value_type());
      it = find(key);
    }

    return it->second;
  }

  iterator begin()
  {
    return _items.begin();
  }

  iterator end()
  {
    return _items.end();
  }

  const_iterator cbegin() const
  {
    return _items.cbegin();
  }

  const_iterator cend() const
  {
    return _items.cend();
  }

  void insert(const key_type& key, const value_type& value)
  {
    if (find(key) != end())
      throw new std::exception("Key already exists in map");

    _items.emplace_back(key, value);
    sortPairs();
  }

  void remove(const key_type& key)
  {
    auto it = find(key);
    if (it == end())
      return;

    _items.erase(it);
  }

  bool empty() const
  {
    return _items.empty();
  }

  size_t size() const
  {
    return _items.size();
  }

private:
  container_type _items;

  void sortPairs()
  {
    comparator_type comparator;
    std::sort(_items.begin(), _items.end(), comparator);
  }
};

