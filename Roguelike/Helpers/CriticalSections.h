/*********************************
 * CriticalSections.h
 * Connor Hilarides
 * Created 2014/08/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "FixedWindows.h"

#define THREAD_EXCLUSIVE_SCOPE \
  static critical_section ___critSec___; \
  critical_section::guard ___critSecGuard___{___critSec___}

class critical_section
{
public:
  critical_section();
  ~critical_section();

  void enter();
  void leave();

  CRITICAL_SECTION *handle();

  class guard
  {
  public:
    guard(critical_section& crit);
    ~guard();

    guard& operator=(const guard&) = delete;

  private:
    critical_section& crit;
  };

  guard lock() { return guard(*this); }

private:
  CRITICAL_SECTION _section;
};

template <typename T>
class object_lock
{
  T obj;
  critical_section lock;

public:
  object_lock() = default;

  explicit object_lock(const T &copy)
    : obj(copy)
  {
  }

  explicit object_lock(T &&move)
    : obj(std::move(move))
  {
  }

  object_lock &operator=(const T &copy)
  {
    auto guard = lock.lock();
    obj = copy;
    return *this;
  }

  object_lock &operator=(T &&move)
  {
    auto guard = lock.lock();
    obj = std::move(move);
    return *this;
  }

  class deref_guard
  {
    const object_lock &lock;
    critical_section::guard guard;

    explicit deref_guard(const object_lock &lock)
      : lock(lock), guard(lock.lock)
    {
    }

    friend class object_lock<T>;

  public:
    const T &operator*()
    {
      return lock.obj;
    }

    const T *operator->()
    {
      return &lock.obj;
    }
  };

  class deref_mut_guard
  {
    object_lock &lock;
    critical_section::guard guard;

    explicit deref_mut_guard(object_lock &lock)
      : lock(lock), guard(lock.lock)
    {
    }

    friend class object_lock<T>;

  public:
    T &operator*()
    {
      return lock.obj;
    }

    T *operator->()
    {
      return &lock.obj;
    }
  };

  deref_guard operator*() const
  {
    return deref_guard(*this);
  }

  deref_mut_guard operator*()
  {
    return deref_mut_guard(*this);
  }

  deref_guard deref() const
  {
    return deref_guard(*this);
  }

  deref_mut_guard deref_mut()
  {
    return deref_mut_guard(*this);
  }
};

template <typename T>
class rwlock
{
  T obj;
  SRWLOCK lock;

  class guard
  {
    rwlock &rwl;
    bool mut;
  public:
    NO_ASSIGNMENT_OPERATOR(guard);

    guard(rwlock &rwl, bool mut)
      : rwl(rwl), mut(mut)
    {
      if (mut)
        AcquireSRWLockExclusive(&rwl.lock);
      else
        AcquireSRWLockShared(&rwl.lock);
    }
    ~guard()
    {
      if (mut)
        ReleaseSRWLockExclusive(&rwl.lock);
      else
        ReleaseSRWLockShared(&rwl.lock);
    }
  };
  
public:
  rwlock()
  {
    InitializeSRWLock(&lock);
  }

  rwlock &operator=(const T &copy)
  {
    auto lock = write();
    *lock = copy;
    return *this;
  }

  rwlock &operator=(T &&move)
  {
    auto lock = write();
    *lock = std::move(move);
    return *this;
  }

  class read_guard
  {
    guard g;
    rwlock &r;

  protected:
    friend class rwlock<T>;
    read_guard(rwlock &r, bool mut = false)
      : g(r, mut), r(r)
    {
    }

    T *objptr() const { return &r.obj; }

  public:
    NO_ASSIGNMENT_OPERATOR(read_guard);

    const T *operator->() const
    {
      return objptr();
    }

    const T &operator*() const
    {
      return *objptr();
    }
  };

  class write_guard : public read_guard
  {
  protected:
    friend class rwlock<T>;
    write_guard(rwlock &r)
      : read_guard(r, true)
    {
    }

  public:
    NO_ASSIGNMENT_OPERATOR(write_guard);

    T *operator->()
    {
      return read_guard::objptr();
    }

    T &operator*()
    {
      return *read_guard::objptr();
    }
  };

  read_guard read()
  {
    return read_guard(*this);
  }

  write_guard write()
  {
    return write_guard(*this);
  }
};

