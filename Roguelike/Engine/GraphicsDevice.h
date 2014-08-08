/*********************************
 * GraphicsDevice.h
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Common.h"

struct WindowCreationOptions;
class GraphicsDevice;
class WindowDevice;

class GraphicsDevice abstract
{
public:
  typedef std::chrono::high_resolution_clock::time_point resource_token_t;

  virtual ~GraphicsDevice();

  virtual HWND getContextWindow() = 0;

  virtual void onResize(std::function<void(math::Vector2D)>) = 0;
  virtual void setSize(math::Vector2D newSize) = 0;
  virtual math::Vector2D getSize() const = 0;

  virtual bool beginFrame() = 0;
  virtual void endFrame() = 0;

  inline bool isResourceInvalid(const resource_token_t& token)
  {
    return token == _deviceCreated;
  }

  static std::unique_ptr<WindowDevice> createWindow(const WindowCreationOptions& options);
  void createInputLayout(byte* bytecode,
                         UINT bytecodeSize,
                         D3D11_INPUT_ELEMENT_DESC* layoutDesc,
                         UINT layoutDescNumElements,
                         ID3D11InputLayout** layout);

protected:
  resource_token_t _deviceCreated;

  PROTECTED_ACCESSIBLE(IDXGISwapChain *, swapChain);
  PROTECTED_ACCESSIBLE(ID3D11Device *, device);
  PROTECTED_ACCESSIBLE(ID3D11DeviceContext *, deviceContext);
  PROTECTED_ACCESSIBLE(ID3D11RenderTargetView *, renderTargetView);
  PROTECTED_ACCESSIBLE(ID3D11Texture2D *, depthStencilBuffer);
  PROTECTED_ACCESSIBLE(ID3D11DepthStencilState *, depthStencilState);
  PROTECTED_ACCESSIBLE(ID3D11DepthStencilView *, depthStencilView);
  PROTECTED_ACCESSIBLE(ID3D11RasterizerState *, rasterState);
  PROTECTED_ACCESSIBLE(ID3D11BlendState *, blendState);

  void initializeD3DContext();
  void freeD3DContext();

public:
  math::Vector backgroundColor;
};

struct WindowCreationOptions
{
  HINSTANCE hInstance;
  math::Vector2D size;
  std::string gameTitle;
};

class WindowDevice final : public GraphicsDevice
{
  HWND _window;
  math::Vector2D _size;

  std::function<void(math::Vector2D)> _onResize;

public:
  HWND getContextWindow() override { return _window; }

  void onResize(std::function<void(math::Vector2D)>) final override;
  void setSize(math::Vector2D newSize) final override;
  math::Vector2D getSize() const final override;

  bool beginFrame() override;
  void endFrame() override;

  void processMessages();

private:
  WindowDevice(const WindowCreationOptions& options);
  static LRESULT CALLBACK staticWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HWND WindowDevice::initializeWindow(const WindowCreationOptions& options);
  LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  friend class GraphicsDevice;
};
