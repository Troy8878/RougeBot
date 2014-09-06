/*********************************
 * AsyncTask.h
 * Connor Hilarides
 * Created 2014/09/05
 *********************************/

#pragma once

#include "CriticalSections.h"
#include <functional>
#include <thread>

// ----------------------------------------------------------------------------

template <typename ReturnT>
class async_task
{
public:
  async_task() = default;
  async_task(const std::function<ReturnT()>& run);
  void start();

  bool has_result();
  ReturnT& get_result();

private:
  void do_threaded();

  struct Internal
  {
    critical_section lock;
    std::thread thread;
    std::function<ReturnT()> run;
    ReturnT result;
    bool has_result;
    bool running;
  };

  std::shared_ptr<Internal> intern;
};

// ----------------------------------------------------------------------------

template <typename Ret, typename funtype, typename... Args>
async_task<Ret> run_async(const funtype& run, Args... args)
{
  std::function<Ret()> func = [run, args...](){ return run(args...); };
  async_task<Ret> task(func);
  task.start();
  return task;
}

// ----------------------------------------------------------------------------

template <typename ReturnT>
async_task<ReturnT>::async_task(const std::function<ReturnT()>& run)
  : intern(std::make_shared<Internal>())
{
  intern->run = run;
  intern->has_result = false;
  intern->running = false;
}

// ----------------------------------------------------------------------------

template <typename ReturnT>
void async_task<ReturnT>::start()
{
  if (!intern)
    throw std::exception("This task does not represent any task instance");

  critical_section::guard guard{intern->lock};

  if (intern->running)
    return;

  intern->thread = std::thread{std::bind(&async_task<ReturnT>::do_threaded, this)};
}

// ----------------------------------------------------------------------------

template <typename ReturnT>
bool async_task<ReturnT>::has_result()
{
  critical_section::guard guard{intern->lock};
  return intern->has_result;
}

// ----------------------------------------------------------------------------

template <typename ReturnT>
ReturnT& async_task<ReturnT>::get_result()
{
  intern->thread.join();
  return intern->result;
}

// ----------------------------------------------------------------------------

template <typename ReturnT>
void async_task<ReturnT>::do_threaded()
{
  intern->lock.enter();
  intern->running = true;
  intern->lock.leave();

  intern->result = intern->run();
  
  critical_section::guard guard{intern->lock};
  intern->has_result = true;
  intern->running = false;

  std::cout << "Async task complete" << std::endl;
}

// ----------------------------------------------------------------------------
