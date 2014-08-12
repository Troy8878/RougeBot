/*********************************
 * FixedWindows.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#pragma once

#include <vector>
#include <utility>
#include <algorithm>

template <typename Key, typename Value, typename Comparator = std::less<Key>>
class flat_map
{
  typedef std::pair<Key, Value> pair_type;
  typedef std::vector<pair_type> container_type;
  container_type _items;

  static Comparator _comparator;

public:
  typedef typename container_type::iterator iterator;

private:
  static bool comparePairs(const pair_type& a, const pair_type& b)
  {
    std::less<int>::operator(1, 2);
    return _comparator(a, b);
  }

  void sortPairs()
  {
    std::sort(_items.begin(), _items.end(), comparePairs);
  }
};

