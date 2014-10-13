/*********************************
 * StackTrace.h
 * Connor Hilarides
 * Created 2014/08/15
 *********************************/

#pragma once

#include <vector>
#include <iostream>

struct trace_line
{
  std::string module;
  std::string method;

  std::string file;
  size_t line = 0;

  void *address;
};

const int max_trace_lines = 100;

class stack_trace
{
public:
  static stack_trace create_trace(int skip = 0);
  static void print_trace(std::ostream& out = std::cerr, const char *indent = "  ");
  void print(std::ostream& out = std::cerr, const char *indent = "  ");

  const std::vector<trace_line>& get_lines() const { return _lines; };

  static trace_line get_trace_line(void *stack_addr);

private:
  stack_trace() = default;

  std::vector<trace_line> _lines;
};

