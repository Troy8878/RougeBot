/*********************************
 * GameConsole.cpp
 * Connor Hilarides
 * Created 2014/09/04
 *********************************/

#include "Common.h"
#include "GameConsole.h"

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
}

// ----------------------------------------------------------------------------

void GameConsole::ExecuteCommand(const std::string& cmd)
{
  if (std::this_thread::get_id() == main_thread)
  {
    ExecuteSyncCommand(cmd);
  }
  else
  {
    THREAD_EXCLUSIVE_SCOPE;
    async_commands.push_back(cmd);
  }
}

// ----------------------------------------------------------------------------

void GameConsole::OnUpdate(Events::EventMessage&)
{
  for (auto& cmd : async_commands)
  {
    ExecuteSyncCommand(cmd);
  }

  async_commands.clear();
}

// ----------------------------------------------------------------------------

void GameConsole::ExecuteSyncCommand(const std::string& cmd)
{
  static ruby::ruby_module kernel{mrb_inst, mrb_inst->mrb_handle()->kernel_module};

  mrb_debug_mbox = false;
  auto result = kernel.functions["eval"].call(cmd).functions["inspect"].call();
  mrb_debug_mbox = true;

  auto pfg = console::fg_color();
  std::cout << console::fg::green
            << "=> "
            << static_cast<std::string>(result)
            << std::endl << pfg;
}

// ----------------------------------------------------------------------------

void GameConsole::CinListen()
{
  std::string cmd;
  while (std::getline(std::cin, cmd))
  {
    ExecuteCommand(cmd);
  }
}

// ----------------------------------------------------------------------------


