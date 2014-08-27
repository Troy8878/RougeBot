/*********************************
 * BucketAllocator.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "Allocator.h"
#include "FixedWindows.h"
#include <vector>

// ----------------------------------------------------------------------------

/**
  Useful for allocating in 'buckets' to improve
  average allocation performance and memory locality
*/
class BucketAllocator : public IAllocator
{
public:
  /**
    itemSize: the sizeof() the type this allocator allocates
    itemCount: the number of items allocated per bucket
  */
  BucketAllocator(size_t itemSize, size_t itemCount = 64);
  ~BucketAllocator() = default;

  void *Allocate() override;
  void Free(void *item) override;

  void Dispose();

  template <typename T, typename... Args>
  T *Create(Args... args)
  {
    if (sizeof(T) > itemSize)
      throw std::bad_alloc();

    T *mem = reinterpret_cast<T *>(Allocate());
    new (mem) T(args...);

    return mem;
  }

  template <typename T>
  void Destroy(T *item)
  {
    item->~T();
    Free(item);
  }

private:
  class Bucket
  {
  public:
    Bucket(BucketAllocator& allocator);
    ~Bucket();

    void *Allocate();
    void Free(void *item);

    bool OwnsItem(void *item);

    PROPERTY(get = _GetEmpty) bool Empty;
    PROPERTY(get = _GetFull) bool Full;

  private:
    Bucket() = default;
    BucketAllocator *_allocator = nullptr;

    byte *memory = nullptr;
    bool *allocations = nullptr;
    size_t contained = 0;
    size_t earlySpot = 0;
    bool empty = true, full = false;

    PROPERTY(get = __getBA) BucketAllocator& allocator;
    inline BucketAllocator& __getBA() { return *_allocator; }

  public:
    inline bool _GetFull() { return full; }
    inline bool _GetEmpty() { return empty; }

    NO_COPY_CONSTRUCTOR(Bucket);
    NO_ASSIGNMENT_OPERATOR(Bucket);

    Bucket(Bucket&& moving);
    Bucket& operator=(Bucket&& moving);

    void swap(Bucket& other);
  };

  Bucket& AddBucket();
  void TrimBuckets();

  size_t itemSize, itemCount;
  std::vector<Bucket> buckets;

  friend void *operator new(size_t size, BucketAllocator& allocator);
};

// ----------------------------------------------------------------------------

