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

    _basicShader = Shader::loadShader(_graphicsDevice.get(), L"BasicVertexShader.cso", L"BasicPixelShader.cso");
    _basicShader->initializeBasicShader();
    _basicCircle = Shapes::makeCircle(_graphicsDevice->device(), 1000000, 5);

    _camera.position = math::Vector{0, 0, 20, 1};
    _camera.lookAt = math::Vector{0, 0, 0, 0};
    _camera.init();
    
    _basicShader->camera = &_camera;
    _basicCircle->setShader(_basicShader);
  }

  void onUpdate(const GameTime& time) override
  {
    using namespace DirectX;
    float dt = (float) time.dt();

    _camera.update();
  }

  void onDraw() override
  {
    using namespace DirectX;

    _basicCircle->draw(_cubeTransform.get());
  }

  void onFree() override
  {
    delete _basicCircle;
    delete _basicShader;
  }

private:
  LookAtCamera _camera;

  Shader *_basicShader;
  Model *_basicCircle;

  math::Matrix _cubeTransform = DirectX::XMMatrixIdentity();
};
