/*********************************
 * Roguelike.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Helpers\UseDirectX.h"

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
  }

  void onInit() override
  {
    using namespace DirectX;

    _graphicsDevice->backgroundColor = XMVectorSet(0, 0, 0, 1);

    //_graphicsDevice->swapChain()->SetFullscreenState(true, nullptr);

    initShaders();
    initObjects();
    initSetup();
  }

  void initShaders()
  {
    _basicShader = Shader::loadShader(
      _graphicsDevice.get(),
      L"Assets/Shaders/BasicVertexShader.cso",
      L"Assets/Shaders/BasicPixelShader.cso");
    _basicShader->initializeBasicShader();

    _textureShader = Shader::loadShader(
      _graphicsDevice.get(),
      L"Assets/Shaders/TexturedVertexShader.cso",
      L"Assets/Shaders/TexturedPixelShader.cso");
    _textureShader->initializeTexturedShader();
  }

  void initObjects()
  {
    _basicCircle = Shapes::makeCircle(_graphicsDevice->device(), 10000, 5);

    _camera.position = math::Vector{0, 0, 60, 1};
    _camera.lookAt = math::Vector{0, 0, 0, 0};
    _camera.init();
  }

  void initSetup()
  {
    _basicShader->camera = &_camera;
    _textureShader->camera = &_camera;
    _basicCircle->shader = _textureShader;
    _basicCircle->texture = Texture2D{_graphicsDevice->device(), L"Assets/Textures/1384108156458.jpg"};
  }

  void onUpdate(const GameTime& time) override
  {
    using namespace DirectX;
    float dt = (float) time.dt();

    // Update FPS
    {
      const int update_res = 120;
      
      static float fps = 0;
      fps = (fps * (update_res - 1) + 1 / dt) / update_res;
      auto title = _title + " [" + std::to_string((int)(fps + 0.5f)) + " fps]";
      SetWindowText(_graphicsDevice->window(), title.c_str());
    }

    _cubeTransform = XMMatrixRotationZ(dt * math::pi) * _cubeTransform.get();

    _camera.position.z() -= dt;
    _camera.update();
  }

  void onDraw() override
  {
    using namespace DirectX;

    _basicCircle->draw(_cubeTransform.get());
    _basicCircle->draw(_cubeTransform.get() *
                       XMMatrixTranslation(10, 0, 0) * 
                       XMMatrixRotationY(-20 * math::pi / 180));
    _basicCircle->draw(_cubeTransform.get() *
                       XMMatrixTranslation(-10, 0, 0) * 
                       XMMatrixRotationY(20 * math::pi / 180));
  }

  void onFree() override
  {
    delete _basicCircle;
    delete _basicShader;
  }

private:
  LookAtCamera _camera;

  Shader *_basicShader;
  Shader *_textureShader;
  Model *_basicCircle;

  math::Matrix _cubeTransform = DirectX::XMMatrixIdentity();
};
