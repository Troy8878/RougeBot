/*********************************
 * Game.cpp
 * Connor Hilarides
 * Created 2014/05/28
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Game.h"
#include "Level.h"
#include "LevelDef.h"
#include "Input.h"
#include "RenderSet.h"
#include "Button.h"

#include "mruby/debug.h"
#include "mruby/variable.h"
#include "WorldSnapshot.h"
#include <PropertyAPI/PropertyServer.h>
#include "Loading.h"

// ----------------------------------------------------------------------------

PropertyServer *propviewer = nullptr;

// ----------------------------------------------------------------------------

static Game *_gameInst;

Game *GetGame()
{
  return _gameInst;
}

// ----------------------------------------------------------------------------

Game::Game(const std::string &title, HINSTANCE hInstance)
  : BasicClassEventReciever(this), _title(title), _hInstance(hInstance)
{
  AI = new AISystem(std::min(std::thread::hardware_concurrency() - 2, 2u));

  GameLock.enter();

  Events::Event::GlobalDispatcher = &globalEventDispatcher;
  _gameInst = this;

  globalEventDispatcher.AddListener(this);
  globalEventDispatcher.AddListener(&ButtonManager::Instance);
  globalEventDispatcher.AddListener(&Input::Instance);
  globalEventDispatcher.AddListener(&levelEventProxy);
}

// ----------------------------------------------------------------------------

Game::~Game()
{
  delete _respack;
}

// ----------------------------------------------------------------------------

void Game::Run()
{
  _running = true;

  _respack = new Respack::ResourcePack(initSettings.assetPack, initSettings.assetFolder);

  ruby::ruby_engine mrb;
  ruby::ruby_engine::global_engine = &mrb;

  Input::Instance.Initialize();
  ButtonManager::Instance.Initialize();
  SoundSystem.Initialize();

  std::shared_ptr<AsyncLoadingScreen> loadingScreen;
  std::thread *screenThread = nullptr;

  try
  {
    _graphicsDevice = GraphicsDevice::CreateGameWindow({_hInstance,{1280, 720}, _title});
    GraphicsOnInit();

    RegisterEngineComponents();

    OnInit();

    // Update
    _gameTime.Update();

    while (_running)
    {
      performance::register_guard glperf("Game Loop");

      performance.wndproc.new_lap();
      _graphicsDevice->ProcessMessages();
      performance.wndproc.end_lap();

#if !PRODUCTION
      performance.unlock_game.new_lap();
      // Provide a window for other threads to do things to the engine
      if (propviewer)
      {
        GameLock.leave();
        Sleep(1);
        GameLock.enter();
      }
      performance.unlock_game.end_lap();
#endif

      performance.load_level.new_lap();
      if (!levelChangeContext.loaded)
      {
        // Load the level
        if (_currentLevel && _currentLevel->Name != "Init")
        {
          if (!loadingScreen)
          {
            loadingScreen = std::make_shared<AsyncLoadingScreen>();
          }
          
          loadingScreen->Init();
          screenThread = new std::thread(
            [loadingScreen]()
            {
              Sleep(100);
              loadingScreen->Run();
            });
        }

        {
          performance::register_guard perf("Level Load");

          if (_currentLevel)
            Level::DestroyLevel(_currentLevel);

          auto newLevel = Level::CreateLevel(levelChangeContext.name);

          _currentLevel = newLevel;
          levelChangeContext.loaded = true;
        }

        // Raise level_load event
        {
          performance::register_guard perf("level_load event");

          using namespace Events;
          static EventId levelLoadId("level_load");
          EventMessage msg{levelLoadId, nullptr, false};
          Event::Raise(msg);
        }
      }
      performance.load_level.end_lap();

      performance.update.new_lap();

      // FMOD Update
      SoundSystem.Update();

      // Raise update event
      if (!_gameTime.Paused)
      {
        performance::register_guard perf("update event");

        using namespace Events;
        static EventId updateId("update");

        UpdateEvent edata{_gameTime};
        EventMessage msg{updateId, &edata, false};
        Event::Raise(msg);
      }
      performance.update.end_lap();

      if (screenThread)
      {
        loadingScreen->Stop();
        screenThread->join();
        screenThread = nullptr;
      }

      // Raise draw event (and draw the frame)
      performance.draw.new_lap();
      if (_graphicsDevice->BeginFrame())
      {
        using namespace Events;
        static EventId drawId("draw");
        EventMessage msg{drawId, nullptr, false};

        // Draw event
        {
          performance::register_guard perf("draw event");
          Event::Raise(msg);
        }

        performance::register_guard perf("drawing");

        // Run the GC while drawing
        std::thread gc_thread
          {
            [&mrb, this]()
            {
              performance.gc.new_lap();
              mrb_full_gc(mrb);
              performance.gc.end_lap();
            }
          };

        // Do the draw
        RenderGroup::Instance.Draw(msg);

        // Draw the wireframes
        if (_graphicsDevice->DebugDraw)
        {
          _graphicsDevice->DeviceContext->RSSetState(_graphicsDevice->WireframeState);
          _graphicsDevice->WireframeDraw = true;

          RenderGroup::Instance.Draw(msg);

          _graphicsDevice->WireframeDraw = false;
          _graphicsDevice->DeviceContext->RSSetState(_graphicsDevice->RasterState);
        }

        // Done :D
        _graphicsDevice->EndFrame();
        performance.draw.end_lap();

        // Update
        _gameTime.Update();

        performance.gc_join.new_lap();
        gc_thread.join();
        performance.gc_join.end_lap();
      }
      else
      {
        performance.draw.end_lap();

        // Update
        _gameTime.Update();
      }

      // Oh no! Zombies!
      performance.entity_kill.new_lap();
      Entity::ExecuteZombies();
      performance.entity_kill.end_lap();
    }

    OnFree();
  }
#if !defined(_DEBUG)
  catch (const basic_exception &ex)
  {
    std::stringstream buf;
    buf << "A fatal exception occurred: ";
    buf << ex.what();
    buf << '\n';
    ex.print_trace(buf);

    MessageBox(nullptr, buf.str().c_str(), nullptr, MB_ICONERROR);
  }
  catch (const std::exception &ex)
  {
    if (&ex == nullptr)
    {
      MessageBox(nullptr, "There is no hope, an exception is null...", nullptr, MB_ICONERROR);
      _exit(-1);
    }

    std::string message = "A fatal exception occurred: ";
    message += ex.what();

    MessageBox(nullptr, message.c_str(), nullptr, MB_ICONERROR);
  }
#else
  catch (void *) // please don't throw these >.>
  {
  }
#endif
}

// ----------------------------------------------------------------------------

void Game::Stop()
{
  _running = false;
}

// ----------------------------------------------------------------------------

void Game::RestartLevel()
{
  levelChangeContext.loaded = false;
}

// ----------------------------------------------------------------------------

void Game::ChangeLevel(const std::string &name)
{
  levelChangeContext.name = name;
  levelChangeContext.loaded = false;
}

// ----------------------------------------------------------------------------

void Game::SetProcHandler(UINT message, wndproc_callback callback)
{
  _wndprocCallbacks[message] = callback;
}

// ----------------------------------------------------------------------------

void Game::GraphicsOnInit()
{
  SetProcHandler(WM_SIZE, [this](HWND, UINT, WPARAM, LPARAM lparam, LRESULT &)
                        {
                          float nx = LOWORD(lparam);
                          float ny = HIWORD(lparam);

                          GameDevice->SetSize({nx, ny});

                          using namespace Events;
                          static EventId eventId("window_resize");

                          RudimentaryEventWrapper<math::Vector2D> data{{nx, ny}};
                          EventMessage message{eventId, &data, false};

                          Event::Raise(message);
                        });
}

// ----------------------------------------------------------------------------

bool Game::LevelEventProxy::CanHandle(const Events::EventMessage &e)
{
  static auto &game = *GetGame();

  return game._currentLevel && game._currentLevel->levelEvents.CanHandle(e);
}

// ----------------------------------------------------------------------------

void Game::LevelEventProxy::Handle(Events::EventMessage &e)
{
  static auto &game = *GetGame();

  game._currentLevel->levelEvents.Handle(e);
}

// ----------------------------------------------------------------------------

