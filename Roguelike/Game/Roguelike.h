/*********************************
 * Roguelike.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#pragma once

#include "Engine\Common.h"

#include "Engine\Game.h"
#include "Engine\Shader.h"
#include "Engine\Model.h"
#include "Engine\Camera.h"
#include "Engine\StandardShapes.h"
#include "Engine\Colors.h"
#include "Engine\RenderSet.h"

#include "mruby/data.h"
#include "mruby/value.h"
#include "mruby/class.h"

// ----------------------------------------------------------------------------

struct asdf
{
  asdf operator+(const asdf& o) const;
};

asdf test = asdf() + asdf();

class Roguelike : public Game
{
public:
  Roguelike(const std::string& title, HINSTANCE hInstance)
    : Game(title, hInstance)
  {
    initSettings.cullTriangles = false;
    initSettings.assetPack = L"./Assets.respack";
    initSettings.assetFolder = L"./Assets";
    initSettings.vsync = true;
  }

  void OnInit() override
  {
    using namespace DirectX;

    _graphicsDevice->backgroundColor = XMVectorSet(1, 0.5, 0, 1);

    //_graphicsDevice->SwapChain->SetFullscreenState(true, nullptr);

    InitShaders();
    InitObjects();

    using namespace Events;
    static EventId updateEvent("update");
    SetHandler(updateEvent, &Roguelike::OnUpdate);

    static EventId drawEvent("draw");
    SetHandler(drawEvent, &Roguelike::OnDraw);

    static EventId resizeEvent("window_resize");
    SetHandler(resizeEvent, &Roguelike::OnResize);

    RenderGroup::Instance.CreateSet("global_hud", &_hudCamera, true);

    _testEntity = EntityFactory::CreateEntity("PancakeFace.entitydef", 
    {
      {
        "SpriteComponent",
        {
          {"render_target", "global_hud"}
        }
      }
    });
    Event::GlobalDispatcher->AddListener(_testEntity);
  }

  void InitShaders()
  {
    Shader::LoadShader(_graphicsDevice.get(), "Basic.shaderdef");
    Shader::LoadShader(_graphicsDevice.get(), "Textured.shaderdef");
  }

  void InitObjects()
  {
    _hudCamera.position = math::Vector{0, 0, 30, 1};
    //_hudCamera.lookAt = math::Vector{0, 0, 0, 1};
    _hudCamera.Init();
    _hudCamera.Update();
  }

  void InitWMHandlers()
  {
    // Keep the window no less wide than a square
    SetProcHandler(WM_GETMINMAXINFO, [this](HWND, UINT, WPARAM, LPARAM lp, LRESULT&)
    {
      MINMAXINFO& info = *reinterpret_cast<LPMINMAXINFO>(lp);

      // TODO: squarify
      
    });
  }

  void OnUpdate(Events::EventMessage& e)
  {
    using namespace Events;
    auto& time = e.GetData<UpdateEvent>()->gameTime;
    float dt = (float) time.Dt;

    UpdateTitleFPS(dt);
  }

  void UpdateTitleFPS(float dt)
  {
    if (dt > 0.001)
    {
      const int update_res = 60;

      static float prev_fps = 60;
      static float fps = 60;
      fps = (fps * (update_res - 1) + 1 / dt) / update_res;

      if (fps != prev_fps)
      {
        prev_fps = fps;

        auto title = _title + " [" + std::to_string((int) (fps + 0.5f)) + " fps]";
        SetWindowText(_graphicsDevice->Window, title.c_str());
      }
    }
  }

  void OnDraw(Events::EventMessage& e)
  {
    RenderGroup::Instance.Draw(e);
  }

  void OnFree() override
  {
    EntityFactory::DestroyEntity(_testEntity);
    delete _basicShader;
    delete _textureShader;
  }

  void OnResize(Events::EventMessage& e)
  {
    using event_type = Events::RudimentaryEventWrapper<math::Vector2D>;
    auto& data = e.GetData<event_type>()->data;

    _hudCamera.size.x = _hudCamera.size.y * data.x / data.y;
    _hudCamera.Init();
    _hudCamera.Update();
  }

private:
  HUDCamera _hudCamera;

  Shader *_basicShader;
  Shader *_textureShader;

  Entity *_testEntity;
};

// ----------------------------------------------------------------------------
