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

    RenderGroup::Instance.CreateSet("global_hud", &_camera, true);

    _testEntity = new Entity;

    component_factory_data tcdata;
    tcdata["position"] = "0 0 0 0";
    tcdata["rotation"] = "0 0 0 0";
    tcdata["scale"] = "<10, 10, 10, 1>";
    _testEntity->AddComponent("TransformComponent", tcdata);

    component_factory_data scdata;
    scdata["texture"] = "1384108156458.jpg";
    scdata["shader"] = "Textured";
    scdata["render_target"] = "global_hud";
    _testEntity->AddComponent("SpriteComponent", scdata);

    component_factory_data stdata;
    stdata["speed"] = "20";
    _testEntity->AddComponent("SpinnyThingComponent", stdata);

    Event::GlobalDispatcher->AddListener(_testEntity);
  }

  void InitShaders()
  {
    _basicShader = Shader::LoadShader(
      _graphicsDevice.get(),
      "BasicVertexShader.cso",
      "BasicPixelShader.cso");
    _basicShader->InitializeBasicShader();
    RegisteredShaders["Basic"] = _basicShader;

    _textureShader = Shader::LoadShader(
      _graphicsDevice.get(),
      "TexturedVertexShader.cso",
      "TexturedPixelShader.cso");
    _textureShader->InitializeTexturedShader();
    RegisteredShaders["Textured"] = _textureShader;
  }

  void InitObjects()
  {
    _camera.position = math::Vector{0, 0, 1000, 1};
    _camera.lookAt = math::Vector{0, 0, 0, 0};
    _camera.Init();
  }

  void OnUpdate(Events::EventMessage& e)
  {
    using namespace Events;
    auto& time = e.GetData<UpdateEvent>()->gameTime;

    using namespace DirectX;
    float dt = (float) time.Dt;

    UpdateTitleFPS(dt);

    if (_camera.position.z > 20)
      _camera.position.z -= dt * _camera.position.z;

    _camera.Update();
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

        auto title = _title + " [" + std::to_string((int)(fps + 0.5f)) + " fps]";
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
    delete _testEntity;
    delete _basicShader;
    delete _textureShader;
  }

  void OnResize(Events::EventMessage& e)
  {
    using event_type = Events::RudimentaryEventWrapper<math::Vector2D>;
    auto& data = e.GetData<event_type>()->data;
    
    _camera.aspectRatio = data.x / data.y;
    _camera.Init();
  }

private:
  LookAtCamera _camera;

  Shader *_basicShader;
  Shader *_textureShader;

  Entity *_testEntity;
};

// ----------------------------------------------------------------------------
