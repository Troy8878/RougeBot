/*********************************
 * Game.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#pragma once

#include "Helpers\FixedWindows.h"
#include "GraphicsDevice.h"
#include "GameTime.h"
#include "EngineInit.h"
#include "EventHandlers.h"

#include "Common.h"

class Game;
class Level;
Game *getGame();

template <typename GameType>
GameType *getGame()
{
  return static_cast<GameType *>(getGame());
}

class Game abstract : public Events::BasicClassEventReciever<Game>
{
public:
  typedef std::function<void(HWND, UINT, WPARAM, LPARAM, LRESULT&)> wndproc_callback;

  Game(const std::string& title, HINSTANCE hInstance);
  virtual ~Game();

  Game& operator=(const Game&) = delete;

  void run();
  void stop() { _running = false; }

  virtual void onInit() {}
  virtual void onFree() {}

  void setProcHandler(UINT message, wndproc_callback callback);

  const GameTime& gameTime() { return _gameTime; }

  GraphicsDevice *getGraphicsDevice() { return _graphicsDevice.get(); };

private:
  std::unordered_map<UINT, wndproc_callback> _wndprocCallbacks;
  GameTime _gameTime;
  bool _running = false;

  void graphicsOnInit();

protected:
  HINSTANCE _hInstance;
  const std::string _title;
  std::unique_ptr<WindowDevice> _graphicsDevice;

public:
  EngineInitSettings initSettings;
  Events::BasicEventDispatcher globalEventDispatcher;

  friend class WindowDevice;
};

