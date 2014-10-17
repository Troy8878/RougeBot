/*********************************
 * BucketAllocator.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#include "BucketAllocator.h"

// ----------------------------------------------------------------------------

BucketAllocator::BucketAllocator(size_t itemSize, size_t itemCount)
  : itemSize(itemSize), itemCount(itemCount)
{
  AddBucket();
}

// ----------------------------------------------------------------------------

auto BucketAllocator::Allocate() -> void *
{
  for (auto& bucket : buckets)
  {
    if (!bucket.Full)
      return bucket.Allocate();
  }

  return AddBucket().Allocate();
}

// ----------------------------------------------------------------------------

void BucketAllocator::Free(void *item)
{
  for (auto& bucket : buckets)
  {
    if (!bucket.OwnsItem(item))
      continue;

    bucket.Free(item);
    TrimBuckets();
    return;
  }

  throw basic_exception("Attempted to free an item that did not belong to this allocator");
}

// ----------------------------------------------------------------------------

void BucketAllocator::Dispose()
{
  buckets.clear();
  AddBucket();
}

// ----------------------------------------------------------------------------

auto BucketAllocator::AddBucket() -> Bucket&
{
  buckets.emplace_back(*this);
  return buckets.end()[-1];
}

// ----------------------------------------------------------------------------

void BucketAllocator::TrimBuckets()
{
  long long freeBuckets[2] = {-1, -1};

  /*
    Basically what we're doing here is checking through the
    list of buckets to see if two of them are empty, and
    storing their IDs. If two of them are empty, we delete
    the second one. This ensures we leave at least one empty
    bucket.
  */
  for (unsigned i = 0; i < buckets.size(); ++i)
  {
    if (buckets[i].Empty)
    {
      if (freeBuckets[0] != -1)
      {
        freeBuckets[1] = i;
        break;
      }

      freeBuckets[0] = i;
    }
  }

  if (freeBuckets[1] == -1)
    return;

  auto it = buckets.begin() + freeBuckets[1];
  buckets.erase(it);
}

// ----------------------------------------------------------------------------

BucketAllocator::Bucket::Bucket(BucketAllocator& _allocator)
  : _allocator(&_allocator)
{
  memory = new byte[allocator.itemSize * allocator.itemCount];
  allocations = new bool[allocator.itemCount];

  for (size_t i = 0; i < allocator.itemCount; ++i)
    allocations[i] = false;
}

// ----------------------------------------------------------------------------

BucketAllocator::Bucket::~Bucket()
{
  delete[] allocations;
  delete[] memory;
}

// ----------------------------------------------------------------------------

auto BucketAllocator::Bucket::Allocate() -> void *
{
  for (size_t i = earlySpot; i < allocator.itemCount; ++i)
  {
    if (allocations[i])
      continue;

    ++contained;
    allocations[i] = true;
    empty = false;
    earlySpot = i + 1;

    if (contained == allocator.itemCount)
      full = true;

    return memory + allocator.itemSize * i;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

void BucketAllocator::Bucket::Free(void *_item)
{
  byte *item = reinterpret_cast<byte *>(_item);
  auto offset = item - memory;
  auto index = offset / allocator.itemSize;

  allocations[index] = false;
  --contained;
  full = false;

  if (index < earlySpot)
    earlySpot = index;

  if (contained == 0)
    empty = true;
}

// ----------------------------------------------------------------------------

auto BucketAllocator::Bucket::OwnsItem(void *_item) -> bool
{
  byte *item = reinterpret_cast<byte *>(_item);
  auto offset = item - memory;
  auto index = offset / (long long) allocator.itemSize;

  return index >= 0 && index < (long long) allocator.itemCount;
}

// ----------------------------------------------------------------------------

BucketAllocator::Bucket::Bucket(Bucket&& moving)
  : Bucket()
{
  swap(moving);
}

// ----------------------------------------------------------------------------

auto BucketAllocator::Bucket::operator=(Bucket&& moving) -> Bucket&
{
  swap(moving);
  return *this;
}

// ----------------------------------------------------------------------------

void BucketAllocator::Bucket::swap(Bucket& other)
{
  std::swap(_allocator, other._allocator);
  std::swap(memory, other.memory);
  std::swap(allocations, other.allocations);
  std::swap(contained, other.contained);
  std::swap(empty, other.empty);
  std::swap(full, other.full);
}

// ----------------------------------------------------------------------------
