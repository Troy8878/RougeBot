/*********************************
 * VisualizePerformance.cpp
 * Connor Hilarides
 * Created 2014/09/21
 *********************************/

#include "Engine/Common.h"
#include "VisualizePerformance.h"
#include "Engine/RubyInterop.h"
#include <vector>
#include <unordered_map>

#ifdef _DEBUG

using namespace performance;

struct perf_entry
{
  clock::time_point start;
  clock::time_point end;
  std::string identifier;

  perf_entry() = default;
  perf_entry(perf_entry&& entry)
    : start(entry.start),
      end(entry.end),
      identifier(std::move(entry.identifier))
  {
  }
};

static struct perf_mgr
{
  std::unordered_map<std::string, clock::duration> totals;

  void add_entry(perf_entry&& entry);

  ~perf_mgr();

} perf;

static std::string method_to_name(mrb_value object, mrb_sym method)
{
  return std::string(
    mrb_class_name(*mrb_inst, mrb_obj_class(*mrb_inst, object))) + 
    '#' + mrb_sym2name(*mrb_inst, method);
}

void performance::register_diagnostic(clock::duration duration, std::string&& identifier)
{
  auto now = clock::now();

  perf_entry entry;
  entry.start = now - duration;
  entry.end = now;
  entry.identifier = std::move(identifier);

  perf.add_entry(std::move(entry));
}

register_guard::register_guard(std::string&& identifier)
  : start(clock::now()), identifier(std::move(identifier))
{
}

register_guard::register_guard(mrb_value object, mrb_sym method)
  : start(clock::now()),
    identifier(method_to_name(object, method))
{
}

register_guard::~register_guard()
{
  auto now = clock::now();

  perf_entry entry;
  entry.start = start;
  entry.end = now;
  entry.identifier = std::move(identifier);

  perf.add_entry(std::move(entry));
}

void perf_mgr::add_entry(perf_entry&& entry)
{
  totals[entry.identifier] += entry.end - entry.start;
}

perf_mgr::~perf_mgr()
{
  std::fstream file{"performance_data.txt"};

  for (auto& pair : totals)
  {
    file << pair.first << ": " << pair.second.count() << std::endl;
  }
}

#endif
