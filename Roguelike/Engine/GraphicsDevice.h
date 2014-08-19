/*********************************
 * GraphicsDevice.h
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

struct WindowCreationOptions;
class GraphicsDevice;
class WindowDevice;

// ----------------------------------------------------------------------------

class GraphicsDevice abstract
{
public:
  typedef std::chrono::high_resolution_clock::time_point resource_token_t;

  virtual ~GraphicsDevice();

  virtual HWND GetContextWindow() = 0;

  virtual void SetSize(math::Vector2D newSize) = 0;
  virtual math::Vector2D GetSize() const = 0;

  virtual bool BeginFrame() = 0;
  virtual void EndFrame() = 0;

  static std::unique_ptr<WindowDevice> CreateGameWindow(const WindowCreationOptions& options);
  void CreateInputLayout(byte* bytecode,
                         UINT bytecodeSize,
                         D3D11_INPUT_ELEMENT_DESC* layoutDesc,
                         UINT layoutDescNumElements,
                         ID3D11InputLayout** layout);

  IR_PROPERTY(IDXGISwapChain *, SwapChain);
  IR_PROPERTY(ID3D11Device *, Device);
  IR_PROPERTY(ID3D11DeviceContext *, DeviceContext);
  IR_PROPERTY(ID3D11RenderTargetView *, RenderTargetView);
  IR_PROPERTY(ID3D11Texture2D *, DepthStencilBuffer);
  IR_PROPERTY(ID3D11DepthStencilState *, DepthStencilState);
  IR_PROPERTY(ID3D11DepthStencilView *, DepthStencilView);
  IR_PROPERTY(ID3D11RasterizerState *, RasterState);
  IR_PROPERTY(ID3D11BlendState *, BlendState);

protected:
  void InitializeD3DContext();
  void FreeD3DContext();

public:
  math::Vector backgroundColor;
};

// ----------------------------------------------------------------------------

struct WindowCreationOptions
{
  HINSTANCE hInstance;
  math::Vector2D size;
  std::string gameTitle;
};

// ----------------------------------------------------------------------------

class WindowDevice final : public GraphicsDevice
{
  math::Vector2D _size;

public:
  HWND GetContextWindow() override { return Window; }

  void SetSize(math::Vector2D newSize) final override;
  math::Vector2D GetSize() const final override;

  bool BeginFrame() override;
  void EndFrame() override;

  void ProcessMessages();

  IR_PROPERTY(HWND, Window);

private:
  WindowDevice(const WindowCreationOptions& options);
  static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HWND InitializeWindow(const WindowCreationOptions& options);
  LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  friend class GraphicsDevice;
};

// ----------------------------------------------------------------------------
