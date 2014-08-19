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

class Roguelike : public Game
{
public:
  Roguelike(const std::string& title, HINSTANCE hInstance)
    : Game{title, hInstance}
  {
    initSettings.cullTriangles = false;
    initSettings.assetPack = L"./Assets.respack";
    initSettings.assetFolder = L"./Assets";
  }

  void OnInit() override
  {
    using namespace DirectX;

    _graphicsDevice->backgroundColor = XMVectorSet(0, 0, 0, 1);

    //_graphicsDevice->swapChain()->SetFullscreenState(true, nullptr);

    InitShaders();
    InitObjects();
    InitSetup();

    using namespace Events;
    static EventId updateEvent("update");
    SetHandler(updateEvent, &Roguelike::OnUpdate);

    static EventId drawEvent("draw");
    SetHandler(drawEvent, &Roguelike::OnDraw);

    static EventId resizeEvent("window_resize");
    SetHandler(resizeEvent, &Roguelike::OnResize);
  }

  void InitShaders()
  {
    _basicShader = Shader::LoadShader(
      _graphicsDevice.get(),
      "BasicVertexShader.cso",
      "BasicPixelShader.cso");
    _basicShader->InitializeBasicShader();

    _textureShader = Shader::LoadShader(
      _graphicsDevice.get(),
      "TexturedVertexShader.cso",
      "TexturedPixelShader.cso");
    _textureShader->InitializeTexturedShader();
  }

  void InitObjects()
  {
    _basicShape = Shapes::MakeRectangle(_graphicsDevice->Device, {10, 10});

    _camera.position = math::Vector{0, 0, 60, 1};
    _camera.lookAt = math::Vector{0, 0, 0, 0};
    _camera.Init();
  }

  void InitSetup()
  {
    _basicShader->camera = &_camera;
    _textureShader->camera = &_camera;
    _basicShape->shader = _textureShader;
    _basicShape->texture = Texture2D{_graphicsDevice->Device, "1384108156458.jpg"};
  }

  void OnUpdate(Events::EventMessage& e)
  {
    using namespace Events;
    auto& time = e.GetData<UpdateEvent>()->gameTime;

    using namespace DirectX;
    float dt = (float) time.Dt;

    // Update FPS
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

    static bool movein = true;
    if (movein)
    {
      _camera.position.z -= dt * 10;
      if (_camera.position.z < 0.5)
        movein = false;
    }
    else
    {
      _camera.position.z += dt * 10;
      if (_camera.position.z > 35)
        movein = true;
    }

    _camera.Update();
  }

  void OnDraw(Events::EventMessage&)
  {
    using namespace DirectX;

    _basicShape->Draw(XMMatrixIdentity());
    _basicShape->Draw(XMMatrixRotationY(-45 * math::pi / 180) *
                      XMMatrixTranslation(9, 0, 4));
    _basicShape->Draw(XMMatrixRotationY(45 * math::pi / 180) *
                      XMMatrixTranslation(-9, 0, 4));
  }

  void OnFree() override
  {
    delete _basicShape;
    delete _basicShader;
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
  Model *_basicShape;
};
