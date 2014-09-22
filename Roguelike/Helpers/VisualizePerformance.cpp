/*********************************
 * VisualizePerformance.cpp
 * Connor Hilarides
 * Created 2014/09/21
 *********************************/

#include "VisualizePerformance.h"
#include "Engine/Common.h"
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
  std::unordered_map<std::string, std::pair<clock::duration, size_t>> totals;

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

performance::register_guard::register_guard(std::string&& identifier)
  : start(clock::now()), identifier(std::move(identifier))
{
}

performance::register_guard::register_guard(mrb_value object, mrb_sym method)
  : start(clock::now()), identifier(method_to_name(object, method))
{
}

performance::register_guard::~register_guard()
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
  auto& total = totals[entry.identifier];
  total.first += entry.end - entry.start;
  total.second++;
}

const long double clock_period = clock::period::num / 
                                 static_cast<long double>(clock::period::den);

perf_mgr::~perf_mgr()
{
  json::value data = json::value::object();

  for (auto& pair : totals)
  {
    json::value entry = json::value::object();

    auto total_time = pair.second.first;
    auto instances = pair.second.second;

    long double d_total = total_time.count() * clock_period;
    long double d_avg = d_total / instances;
    long double d_inst = static_cast<long double>(instances);

    entry["total time"] = json::value{d_total};
    entry["average time"] = json::value{d_avg};
    entry["times ran"] = json::value{d_inst};

    data[pair.first] = entry;
  }
  
  std::ofstream file{"performance_data.txt"};
  data.pretty_print(file);
}

#endif
