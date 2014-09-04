/*********************************
 * GameConsole.h
 * Connor Hilarides
 * Created 2014/09/04
 *********************************/

#pragma once

#include "Engine/Common.h"
#include <thread>

// ----------------------------------------------------------------------------

class GameConsole : public Events::BasicClassEventReciever<GameConsole>
{
public:
  GameConsole(bool listenCmd);
  ~GameConsole();

  void ExecuteCommand(const std::string& cmd);

private:
  std::thread::id main_thread;
  std::thread cin_listen_thread;
  std::vector<std::string> async_commands;

  void OnUpdate(Events::EventMessage& e);
  void ExecuteSyncCommand(const std::string& cmd);
  void CinListen();
};

// ----------------------------------------------------------------------------


