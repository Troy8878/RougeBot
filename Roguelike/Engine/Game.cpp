/*********************************
 * Game.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Game.h"
#include "Level.h"

// ----------------------------------------------------------------------------

static Game *_gameInst;
Game *getGame()
{
  return _gameInst;
}

// ----------------------------------------------------------------------------

Game::Game(const std::string& title, HINSTANCE hInstance)
  : BasicClassEventReciever(this), _title(title), _hInstance(hInstance)
{
  Events::Event::globalDispatcher = &globalEventDispatcher;
  _gameInst = this;

  globalEventDispatcher.addListener(this);
  globalEventDispatcher.addListener(&levelEventProxy);
}

// ----------------------------------------------------------------------------

Game::~Game()
{
  delete _respack;
}

// ----------------------------------------------------------------------------

void Game::run()
{
  _running = true;

  _respack = new Respack::ResourcePack(initSettings.assetPack, initSettings.assetFolder);

  try
  {
    _graphicsDevice = GraphicsDevice::createWindow({_hInstance, {1280, 720}, _title});
    graphicsOnInit();

    onInit();

    while (_running)
    {
      _graphicsDevice->processMessages();

      // Update
      _gameTime.update();

      // Raise update event
      {
        using namespace Events;
        static event_id updateId = Event::createEventId("update");
        UpdateEvent edata{_gameTime};
        EventMessage msg{updateId, &edata, false};

        Event::raise(msg);
      }

      // Raise draw event (and draw the frame)
      if (_graphicsDevice->beginFrame())
      {
        using namespace Events;
        static event_id drawId = Event::createEventId("draw");
        EventMessage msg{drawId, nullptr, false};

        Event::raise(msg);

        _graphicsDevice->endFrame();
      }
    }
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

void Game::setProcHandler(UINT message, wndproc_callback callback)
{
  _wndprocCallbacks[message] = callback;
}

// ----------------------------------------------------------------------------

void Game::graphicsOnInit()
{
  setProcHandler(WM_SIZE, [this](HWND, UINT, WPARAM, LPARAM lparam, LRESULT&)
  {
    float nx = LOWORD(lparam);
    float ny = HIWORD(lparam);
    
    this->getGraphicsDevice()->setSize({nx, ny});

    static event_id eventId = Events::Event::createEventId("window_resize");
    Events::RudimentaryEventWrapper<math::Vector2D> data{{nx, ny}};
    Events::EventMessage message{eventId, &data, false};
    Events::Event::raise(message);
  });
}

// ----------------------------------------------------------------------------

bool Game::LevelEventProxy::canHandle(const Events::EventMessage& e)
{
  static auto& game = *getGame();

  return game._currentLevel && game._currentLevel->levelEvents.canHandle(e);
}

// ----------------------------------------------------------------------------

void Game::LevelEventProxy::handle(Events::EventMessage& e)
{
  static auto& game = *getGame();

  game._currentLevel->levelEvents.handle(e);
}

// ----------------------------------------------------------------------------
