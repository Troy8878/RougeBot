/*********************************
 * GameConsole.cpp
 * Connor Hilarides
 * Created 2014/09/04
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "GameConsole.h"

// ----------------------------------------------------------------------------

MrbAryStreamBuffer consoleStreamBuf;

// ----------------------------------------------------------------------------

EXTERN_C void gwrite(const void *str, size_t size, size_t ct)
{
  std::cout.write(reinterpret_cast<const char *>(str), size * ct);
}

EXTERN_C void gputc(int c)
{
  std::cout << static_cast<char>(c);
}

// ----------------------------------------------------------------------------

GameConsole::GameConsole(bool listenCmd)
  : event_base(this), main_thread(std::this_thread::get_id())
{
  if (listenCmd)
  {
    cin_listen_thread = std::thread{std::bind(&GameConsole::CinListen, this)};
  }

  static Events::EventId update{"update"};
  event_base::SetHandler(update, &GameConsole::OnUpdate);
}

// ----------------------------------------------------------------------------

GameConsole::~GameConsole()
{
  cin_listen_thread.detach();
  stop_getline_async = true;
}

// ----------------------------------------------------------------------------

static critical_section command_section;

void GameConsole::ExecuteCommand(const std::string& cmd)
{
  if (std::this_thread::get_id() == main_thread)
  {
    ExecuteSyncCommand(cmd);
  }
  else
  {
    critical_section::guard cg{command_section};
    async_commands.push_back(cmd);
  }
}

// ----------------------------------------------------------------------------

void GameConsole::OnUpdate(Events::EventMessage&)
{
  critical_section::guard cg{command_section};

  for (auto& cmd : async_commands)
  {
    ExecuteSyncCommand(cmd);
  }

  async_commands.clear();
}

// ----------------------------------------------------------------------------

void GameConsole::ExecuteSyncCommand(const std::string& cmd)
{
  ruby::ruby_gc_guard gcguard{*mrb_inst};

  static ruby::ruby_module kernel{mrb_inst, mrb_inst->mrb_handle()->kernel_module};

  mrb_debug_mbox = false;

  auto result = kernel.functions["eval"].call(cmd).functions["inspect"].call();

  auto pfg = console::fg_color();
  std::cout << console::fg::green
            << "=> "
            << static_cast<std::string>(result)
            << std::endl << pfg;

  mrb_debug_mbox = true;
}

// ----------------------------------------------------------------------------

void GameConsole::CinListen()
{
  std::string cmd;
  while (!stop_getline_async)
  {
    if (getline_async(cmd, std::chrono::milliseconds{100}))
      ExecuteCommand(cmd);
  }
}

// ----------------------------------------------------------------------------

static mrb_data_type streambuf_dt;

static mrb_value mrb_ary_streambuf_new(mrb_state *mrb, MrbAryStreamBuffer *buf);
static mrb_value mrb_ary_streambuf_gc(mrb_state *mrb, mrb_value self);
static mrb_value mrb_ary_streambuf_peek(mrb_state *mrb, mrb_value self);
static mrb_value mrb_ary_streambuf_flush(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

mrb_value MrbAryStreamBuffer::GetRubyWrapper()
{
  return mrb_ary_streambuf_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_ary_streambuf_init(mrb_state *mrb)
{
  ruby::data_type_init<MrbAryStreamBuffer>(streambuf_dt);
  
  auto *cls = mrb_define_class(mrb, "AryStreamBuffer", mrb->object_class);
  mrb_define_class_method(mrb, cls, "game_console", mrb_ary_streambuf_gc, ARGS_NONE());
  mrb_define_method(mrb, cls, "peek", mrb_ary_streambuf_peek, ARGS_NONE());
  mrb_define_method(mrb, cls, "flush", mrb_ary_streambuf_flush, ARGS_NONE());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_ary_streambuf_new(mrb_state *mrb, MrbAryStreamBuffer *buf)
{
  return mrb_obj_value(
    mrb_data_object_alloc(
      mrb, mrb_class_get(mrb, "AryStreamBuffer"),
      buf, &streambuf_dt));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_ary_streambuf_gc(mrb_state *, mrb_value)
{
  auto *buf = dynamic_cast<MrbAryStreamBuffer *>(std::cout.rdbuf());
  if (!buf)
    return mrb_nil_value();
  return buf->GetRubyWrapper();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_ary_streambuf_peek(mrb_state *mrb, mrb_value self)
{
  auto *buf = ruby::data_get<MrbAryStreamBuffer>(mrb, self);
  return buf->PeekAry();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_ary_streambuf_flush(mrb_state *mrb, mrb_value self)
{
  auto *buf = ruby::data_get<MrbAryStreamBuffer>(mrb, self);
  return buf->FlushAry();
}

// ----------------------------------------------------------------------------
