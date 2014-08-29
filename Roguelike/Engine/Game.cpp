/*********************************
 * Game.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Game.h"
#include "Level.h"

#include "mruby/debug.h"

// ----------------------------------------------------------------------------

static Game *_gameInst;
Game *GetGame()
{
  return _gameInst;
}

// ----------------------------------------------------------------------------

Game::Game(const std::string& title, HINSTANCE hInstance)
  : BasicClassEventReciever(this), _title(title), _hInstance(hInstance)
{
  Events::Event::GlobalDispatcher = &globalEventDispatcher;
  _gameInst = this;

  globalEventDispatcher.AddListener(this);
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

  try
  {
    _graphicsDevice = GraphicsDevice::CreateGameWindow({_hInstance, {1280, 720}, _title});
    GraphicsOnInit();

    RegisterEngineComponents();

    OnInit();

    while (_running)
    {
      _graphicsDevice->ProcessMessages();

      // Update
      _gameTime.Update();

      // Raise update event
      {
        using namespace Events;
        static EventId updateId("update");

        UpdateEvent edata{_gameTime};
        EventMessage msg{updateId, &edata, false};
        Event::Raise(msg);
      }

      // Raise draw event (and draw the frame)
      if (_graphicsDevice->BeginFrame())
      {
        using namespace Events;
        static EventId drawId("draw");

        EventMessage msg{drawId, nullptr, false};
        Event::Raise(msg);

        _graphicsDevice->EndFrame();
      }

      mrb_incremental_gc(mrb);
    }

    OnFree();
  }
#if !defined(_DEBUG)
  catch(const std::exception& ex)
  {
    std::string message = "A fatal exception occurred: ";
    message += ex.what();

    MessageBox(NULL, message.c_str(), NULL, MB_ICONERROR);
  }
#else
  catch(void *) // please don't throw these >.>
  {
  }
#endif
}

// ----------------------------------------------------------------------------

void Game::SetProcHandler(UINT message, wndproc_callback callback)
{
  _wndprocCallbacks[message] = callback;
}

// ----------------------------------------------------------------------------

void Game::GraphicsOnInit()
{
  SetProcHandler(WM_SIZE, [this](HWND, UINT, WPARAM, LPARAM lparam, LRESULT&)
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

bool Game::LevelEventProxy::CanHandle(const Events::EventMessage& e)
{
  static auto& game = *GetGame();

  return game._currentLevel && game._currentLevel->levelEvents.CanHandle(e);
}

// ----------------------------------------------------------------------------

void Game::LevelEventProxy::Handle(Events::EventMessage& e)
{
  static auto& game = *GetGame();

  game._currentLevel->levelEvents.Handle(e);
}

// ----------------------------------------------------------------------------
