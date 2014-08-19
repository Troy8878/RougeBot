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

  void onInit() override
  {
    using namespace DirectX;

    _graphicsDevice->backgroundColor = XMVectorSet(0, 0, 0, 1);

    //_graphicsDevice->swapChain()->SetFullscreenState(true, nullptr);

    initShaders();
    initObjects();
    initSetup();

    static event_id updateEvent = Events::Event::createEventId("update");
    setHandler(updateEvent, &Roguelike::onUpdate);

    static event_id drawEvent = Events::Event::createEventId("draw");
    setHandler(drawEvent, &Roguelike::onDraw);

    static event_id resizeEvent = Events::Event::createEventId("window_resize");
    setHandler(resizeEvent, &Roguelike::onResize);
  }

  void initShaders()
  {
    _basicShader = Shader::loadShader(
      _graphicsDevice.get(),
      "BasicVertexShader.cso",
      "BasicPixelShader.cso");
    _basicShader->initializeBasicShader();

    _textureShader = Shader::loadShader(
      _graphicsDevice.get(),
      "TexturedVertexShader.cso",
      "TexturedPixelShader.cso");
    _textureShader->initializeTexturedShader();
  }

  void initObjects()
  {
    _basicShape = Shapes::makeRectangle(_graphicsDevice->device, {10, 10});

    _camera.position = math::Vector{0, 0, 60, 1};
    _camera.lookAt = math::Vector{0, 0, 0, 0};
    _camera.init();
  }

  void initSetup()
  {
    _basicShader->camera = &_camera;
    _textureShader->camera = &_camera;
    _basicShape->shader = _textureShader;
    _basicShape->texture = Texture2D{_graphicsDevice->device, "1384108156458.jpg"};
  }

  void onUpdate(Events::EventMessage& e)
  {
    using namespace Events;
    auto& time = e.getData<UpdateEvent>()->gameTime;

    using namespace DirectX;
    float dt = (float) time.dt();

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
        SetWindowText(_graphicsDevice->window, title.c_str());
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

    _camera.update();
  }

  void onDraw(Events::EventMessage&)
  {
    using namespace DirectX;

    _basicShape->draw(XMMatrixIdentity());
    _basicShape->draw(XMMatrixRotationY(-45 * math::pi / 180) *
                      XMMatrixTranslation(9, 0, 4));
    _basicShape->draw(XMMatrixRotationY(45 * math::pi / 180) *
                      XMMatrixTranslation(-9, 0, 4));
  }

  void onFree() override
  {
    delete _basicShape;
    delete _basicShader;
  }

  void onResize(Events::EventMessage& e)
  {
    using event_type = Events::RudimentaryEventWrapper<math::Vector2D>;
    auto& data = e.getData<event_type>()->data;
    
    _camera.aspectRatio = data.x / data.y;
    _camera.init();
  }

private:
  LookAtCamera _camera;

  Shader *_basicShader;
  Shader *_textureShader;
  Model *_basicShape;
};
