/*********************************
 * Game.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#pragma once

#include "Common.h"

#include "GraphicsDevice.h"
#include "EngineInit.h"
#include "EventHandlers.h"
#include "RubyInterop.h"
#include "ResourceReader\ResourcePack.h"
#include "Helpers\AsyncTask.h"

// ----------------------------------------------------------------------------

class Game;
class Level;
Game *GetGame();

// ----------------------------------------------------------------------------

template <typename GameType>
GameType *GetGame()
{
  return static_cast<GameType *>(getGame());
}

// ----------------------------------------------------------------------------

static ruby::ruby_engine *const& mrb_inst = ruby::ruby_engine::global_engine;

// ----------------------------------------------------------------------------

class Game abstract : public Events::BasicClassEventReciever<Game>
{
public:
  typedef std::function<void(HWND, UINT, WPARAM, LPARAM, LRESULT&)> wndproc_callback;

  Game(const std::string& title, HINSTANCE hInstance);
  virtual ~Game();

  Game& operator=(const Game&) = delete;

  void Run();
  void Stop() { _running = false; }

  virtual void OnInit() {}
  virtual void OnFree() {}

  void RestartLevel();
  void ChangeLevel(const std::string& name)
  {
    levelChangeContext.name = name;
    levelChangeContext.loaded = false;
  }

  void SetProcHandler(UINT message, wndproc_callback callback);

  void SetProcHandlers(wndproc_callback) {}

  template <typename... MSGs>
  void SetProcHandlers(wndproc_callback callback, UINT msg, MSGs... msgs)
  {
    SetProcHandler(msg, callback);
    SetProcHandlers(callback, msgs...);
  }

  PROPERTY(get = __getGameTime) const GameTime& Time;
  PROPERTY(get = __getGraphicsDevice) GraphicsDevice *GameDevice;
  PROPERTY(get = __getRespack) Respack::ResourcePack& Respack;
  PROPERTY(get = __getCurrLevel) Level *CurrentLevel;

private:
  std::unordered_map<UINT, wndproc_callback> _wndprocCallbacks;
  GameTime _gameTime;
  Respack::ResourcePack *_respack = nullptr;
  Level *_currentLevel = nullptr;
  bool _running = false;

  void GraphicsOnInit();

protected:
  HINSTANCE _hInstance;
  const std::string _title;
  std::unique_ptr<WindowDevice> _graphicsDevice;

public:
  EngineInitSettings initSettings;
  Events::BasicEventDispatcher globalEventDispatcher;

  struct LevelEventProxy : public Events::EventReciever
  {
    bool CanHandle(const Events::EventMessage& e) override;
    void Handle(Events::EventMessage& e) override;

  } levelEventProxy;

  struct LevelChangeContext
  {
    std::string name;
    bool loaded = false;

  } levelChangeContext;
  
  inline const GameTime& __getGameTime() { return _gameTime; }
  inline GraphicsDevice *__getGraphicsDevice() { return _graphicsDevice.get(); }
  inline Respack::ResourcePack& __getRespack() { return *_respack; }
  inline Level *__getCurrLevel() { return _currentLevel; }

  friend class WindowDevice;
};

// ----------------------------------------------------------------------------

