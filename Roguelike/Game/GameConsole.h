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

const size_t GameConsoleMaxLine = 127;

class MrbAryStreamBuffer : public std::streambuf
{
public:
  MrbAryStreamBuffer()
    : ary(mrb_nil_value())
  {
  }

  ~MrbAryStreamBuffer()
  {
    if (!mrb_nil_p(ary))
    {
      GCUnlockObj(gclock);
    }
  }

  mrb_value PeekAry()
  {
    return this->ary;
  }
  
  mrb_value FlushAry()
  {
    mrb_value ary = this->ary;

    GCUnlockObj(gclock);
    this->ary = mrb_ary_new(*mrb_inst);
    gclock = GCLockObj(this->ary);

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

    if (currPos == 0)
      color = static_cast<mrb_int>(console::fg_color());
    
    currLine[currPos++] = static_cast<char>(c);

    return c;
  }

  bool VerifyAry()
  {
    if (!mrb_inst)
      return false;
    if (!mrb_nil_p(ary))
      return true;

    ary = mrb_ary_new(*mrb_inst);
    gclock = GCLockObj(ary);
    return true;
  }

  void FlushLine()
  {
    mrb_state *mrb = *mrb_inst;

    static mrb_sym color_s = mrb_intern_lit(mrb, "color");
    static mrb_sym text_s = mrb_intern_lit(mrb, "text");

    mrb_value hsh = mrb_hash_new(mrb);
    mrb_hash_set(mrb, hsh, mrb_symbol_value(color_s), mrb_fixnum_value(color));
    mrb_hash_set(mrb, hsh, mrb_symbol_value(text_s), mrb_str_new(mrb, currLine, currPos));
    mrb_ary_push(mrb, ary, hsh);

    currPos = 0;
  }

private:
  char currLine[GameConsoleMaxLine + 1];
  size_t currPos = 0;
  mrb_value ary;
  mrb_int color;
  mrb_int gclock;
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