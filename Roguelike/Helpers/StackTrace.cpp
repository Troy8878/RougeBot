/*********************************
 * StackTrace.cpp
 * Connor Hilarides
 * Created 2014/08/15
 *********************************/

#include "StackTrace.h"
#include "CStackTrace.h"
#include "FixedWindows.h"
#include "FileSystem.h"
#include <DbgHelp.h>
#include <regex>
#include <sstream>

#pragma comment(lib, "dbghelp.lib")

stack_trace stack_trace::create_trace(int skip)
{
  stack_trace trace;
  void *raw_trace[max_trace_lines];
  auto num_captured = CaptureStackBackTrace(1 + skip, max_trace_lines, raw_trace, NULL);

  for (decltype(num_captured) i = 0; i < num_captured; ++i)
  {
    trace._lines.push_back(get_trace_line(raw_trace[i]));
  }

  return trace;
}

void stack_trace::print_trace(std::ostream& out, const char *indent)
{
  auto trace = create_trace(1);
  for (auto& line : trace.get_lines())
  {
    out << indent;
    if (line.module.length())
      out << fs::path{line.module}.filename() << "!";
    out << line.method << " (" 
        << fs::path{line.file}.filename() << ":" 
        << line.line << ")" << std::endl;
  }
}

static void init_sym()
{
  static bool initialized = false;
  if (initialized)
    return;

  SymInitialize(GetCurrentProcess(), NULL, true);
  initialized = true;
}

trace_line stack_trace::get_trace_line(void *stack_addr)
{
  init_sym();

  DWORD64 dwDisplacement = 0;
  DWORD64 dwAddress = reinterpret_cast<DWORD64>(stack_addr);

  char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
  PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(buffer);

  pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  pSymbol->MaxNameLen = MAX_SYM_NAME;

  trace_line trace;
  trace.address = stack_addr;

  if (SymFromAddr(GetCurrentProcess(), dwAddress, &dwDisplacement, pSymbol))
  {
    trace.method = pSymbol->Name;

    HMODULE module;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                           (LPCSTR) pSymbol->ModBase, &module))
    {
      char moduleName[MAX_PATH];

      if (GetModuleFileNameA(module, moduleName, ARRAYSIZE(moduleName)))
      {
        trace.module = moduleName;
      }
    }
  }
  else
  {
    _com_error e(GetLastError());
    trace.method = std::string{"Error: "} + e.ErrorMessage();
  }

  DWORD dwLineDisplacement;
  IMAGEHLP_LINE64 line;
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
  
  if (SymGetLineFromAddr64(GetCurrentProcess(), dwAddress, &dwLineDisplacement, &line))
  {
    trace.file = line.FileName;
    trace.line = line.LineNumber;
  }

  return trace;
}

extern "C" struct st_trace *st_get_trace(void *addr)
{
  auto line = stack_trace::get_trace_line(addr);
  auto *trace = new st_trace;

  std::ostringstream sfile;

  std::regex path_begin{"(.*)Roguelike\\\\", std::regex::icase};
  std::regex_replace(std::ostreambuf_iterator<char>(sfile), 
                     line.file.begin(), line.file.end(), 
                     path_begin, "");

  line.file = sfile.str();

  size_t fsize = line.file.size() + 1;
  size_t nsize = line.method.size() + 1;

  trace->file = new char[fsize];
  trace->name = new char[nsize];
  trace->line = (int) line.line;
  
  strcpy_s(trace->file, fsize, line.file.c_str());
  strcpy_s(trace->name, nsize, line.method.c_str());

  return trace;
}

extern "C" void st_free_trace(struct st_trace *trace)
{
  delete[] trace->file;
  delete[] trace->name;
  delete trace;
}

