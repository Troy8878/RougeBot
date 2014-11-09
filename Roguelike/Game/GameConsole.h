/*********************************
 * GameConsole.h
 * Connor Hilarides
 * Created 2014/09/04
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Engine/Common.h"
#include <thread>

// ----------------------------------------------------------------------------

EXTERN_C void gwrite(const void *str, size_t size, size_t ct);
EXTERN_C void gputc(int c);

// ----------------------------------------------------------------------------

const size_t GameConsoleMaxLine = 100;

class MrbAryStreamBuffer : public std::streambuf
{
public:
  MrbAryStreamBuffer()
  {
  }

  mrb_value PeekAry()
  {
    return this->ary;
  }
  
  mrb_value FlushAry()
  {
    mrb_value ary = this->ary;
    this->ary = mrb_ary_new(*mrb_inst);
    return ary;
  }

  mrb_value GetRubyWrapper();

  std::streambuf *extra = nullptr;

protected:
  virtual int_type overflow(int_type c) override
  {
    if (extra)
      extra->sputc(static_cast<char>(c));

    if (!VerifyAry())
      return c;

    if (c == '\n')
    {
      FlushLine();
      return c;
    }

    if (currPos == GameConsoleMaxLine)
      FlushLine();
    
    currLine[currPos++] = static_cast<char>(c);

    return c;
  }

  bool VerifyAry()
  {
    if (!mrb_inst)
      return false;

    ary = mrb_ary_new(*mrb_inst);
    return true;
  }

  void FlushLine()
  {
    mrb_state *mrb = *mrb_inst;
    mrb_ary_push(mrb, ary, mrb_str_new(mrb, currLine, currPos));

    currPos = 0;
  }

private:
  char currLine[GameConsoleMaxLine];
  size_t currPos = 0;
  mrb_value ary;
};

extern MrbAryStreamBuffer consoleStreamBuf;

// ----------------------------------------------------------------------------

class GameConsole : public Events::BasicClassEventReciever<GameConsole>
{
public:
  explicit GameConsole(bool listenCmd);
  ~GameConsole();

  void ExecuteCommand(const std::string &cmd);

private:
  std::thread::id main_thread;
  std::thread cin_listen_thread;
  std::vector<std::string> async_commands;

  void OnUpdate(Events::EventMessage &e);
  void ExecuteSyncCommand(const std::string &cmd);
  void CinListen();
};

// ----------------------------------------------------------------------------