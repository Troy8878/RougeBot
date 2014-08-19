/*********************************
 * GraphicsDevice.cpp
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#include "Common.h"
#include "GraphicsDevice.h"
#include "Helpers\Exceptions.h"
#include "Engine\Game.h"

// ----------------------------------------------------------------------------

GraphicsDevice::~GraphicsDevice()
{
  FreeD3DContext();
}

// ----------------------------------------------------------------------------

void GraphicsDevice::FreeD3DContext()
{
  ReleaseDXInterface(RasterState);
  ReleaseDXInterface(DepthStencilView);
  ReleaseDXInterface(DepthStencilState);
  ReleaseDXInterface(DepthStencilBuffer);
  ReleaseDXInterface(RenderTargetView);
  ReleaseDXInterface(DeviceContext);
  ReleaseDXInterface(Device);
  ReleaseDXInterface(SwapChain);
}

// ----------------------------------------------------------------------------

std::unique_ptr<WindowDevice> GraphicsDevice::CreateGameWindow(const WindowCreationOptions& options)
{
  auto *window = new WindowDevice(options);
  return std::unique_ptr < WindowDevice > {window};
}

// ----------------------------------------------------------------------------

HWND WindowDevice::InitializeWindow(const WindowCreationOptions& options)
{
  std::string className = std::to_string(reinterpret_cast<size_t>(this));

  WNDCLASSEX wndc = {sizeof(wndc)};
  wndc.cbWndExtra = sizeof(WindowDevice *);
  wndc.hInstance = options.hInstance;
  wndc.lpfnWndProc = StaticWindowProc;
  wndc.lpszClassName = className.c_str();
  wndc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
  RegisterClassEx(&wndc);

  HWND window = CreateWindow(wndc.lpszClassName,
                             options.gameTitle.c_str(),
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             static_cast<UINT>(options.size.x),
                             static_cast<UINT>(options.size.y),
                             NULL, NULL, options.hInstance,
                             reinterpret_cast<void *>(this));

  ShowWindow(window, SW_SHOWNORMAL);
  UpdateWindow(window);

  return window;
}

// ----------------------------------------------------------------------------

WindowDevice::WindowDevice(const WindowCreationOptions& options)
  : _size(options.size)
{
  Window = InitializeWindow(options);

  InitializeD3DContext();
}

// ----------------------------------------------------------------------------

LRESULT CALLBACK WindowDevice::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  if (msg == WM_CREATE)
  {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, lparam);
    return 0;
  }

  auto *_this = reinterpret_cast<WindowDevice *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  return _this->WindowProc(hwnd, msg, wparam, lparam);
}

// ----------------------------------------------------------------------------

LRESULT WindowDevice::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  auto& game = *GetGame();
  auto iter = game._wndprocCallbacks.find(msg);
  if (iter != game._wndprocCallbacks.end())
  {
    auto& handler = iter->second;

    LRESULT result = 0;
    handler(hwnd, msg, wparam, lparam, result);
    return result;
  }

  switch (msg)
  {
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0;
    }

    case WM_CLOSE:
    {
      GetGame()->Stop();
      return 0;
    }
  }

  return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ----------------------------------------------------------------------------

void WindowDevice::ProcessMessages()
{
  MSG message;
  while (PeekMessage(&message, Window, 0, 0, TRUE))
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}

// ----------------------------------------------------------------------------

void WindowDevice::SetSize(math::Vector2D size)
{
  if (!SwapChain)
    return;

  // Release render target
  DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
  ReleaseDXInterface(RenderTargetView);

  HRESULT hr;
  hr = SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
  CHECK_HRESULT(hr);

  ID3D11Texture2D *pBuffer;
  hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            reinterpret_cast<void **>(&pBuffer));
  CHECK_HRESULT(hr);

  hr = Device->CreateRenderTargetView(pBuffer, nullptr, &RenderTargetView);
  ReleaseDXInterface(pBuffer);

  DeviceContext->OMSetRenderTargets(1, &RenderTargetView, nullptr);

  D3D11_VIEWPORT vp;
  vp.Width = size.x;
  vp.Height = size.y;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  DeviceContext->RSSetViewports(1, &vp);

  _size = size;
}

// ----------------------------------------------------------------------------

math::Vector2D WindowDevice::GetSize() const
{
  return _size;
}

// ----------------------------------------------------------------------------

bool WindowDevice::BeginFrame()
{
  if (DeviceContext == nullptr)
    return false;

  DeviceContext->ClearRenderTargetView(RenderTargetView, backgroundColor.buffer());
  DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  return true;
}

// ----------------------------------------------------------------------------

void WindowDevice::EndFrame()
{
  static bool vsync = GetGame()->initSettings.vsync;
  if (vsync)
  {
    SwapChain->Present(1, 0);
  }
  else
  {
    const double min_frame_time = 0.001;

    static auto& time = GetGame()->Time;
    while (time.CurrFrameTime < min_frame_time)
      Sleep(0);

    SwapChain->Present(0, 0);
  }
}

// ----------------------------------------------------------------------------

void GraphicsDevice::InitializeD3DContext()
{
  auto contextSize = GetSize();
  auto _window = GetContextWindow();
  CHECK_HRESULT(GetLastError());

#pragma region Initialize Swap Chain

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 1;
  sd.BufferDesc.Width = static_cast<UINT>(contextSize.x);
  sd.BufferDesc.Height = static_cast<UINT>(contextSize.y);
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // I don't even, it's what MSDN told me to use >.>
  sd.BufferDesc.RefreshRate.Numerator = 120;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = _window;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

#pragma endregion

#pragma region Create Device and Swap Chain

  D3D_FEATURE_LEVEL  FeatureLevelsRequested[] =
  {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };
  UINT               numLevelsRequested = ARRAYSIZE(FeatureLevelsRequested);
  D3D_FEATURE_LEVEL  FeatureLevelSupported;

  // this is a big function call >.> just go to
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ff476879(v=vs.85).aspx
  // if you have no idea, because it's too complex to describe here
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    0,
    FeatureLevelsRequested,
    numLevelsRequested,
    D3D11_SDK_VERSION,
    &sd,
    &SwapChain,
    &Device,
    &FeatureLevelSupported,
    &DeviceContext);
  CHECK_HRESULT(hr);

  ID3D11Texture2D *backBuffer;
  hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), 
                            reinterpret_cast<LPVOID *>(&backBuffer));
  CHECK_HRESULT(hr);

  hr = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
  CHECK_HRESULT(hr);

  ReleaseDXInterface(backBuffer);

#pragma endregion

#pragma region Depth Stencil Buffer

  D3D11_TEXTURE2D_DESC depthBufferDesc;
  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
  depthBufferDesc.Width = static_cast<UINT>(contextSize.x);
  depthBufferDesc.Height = static_cast<UINT>(contextSize.y);
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  hr = Device->CreateTexture2D(&depthBufferDesc, nullptr, &DepthStencilBuffer);
  CHECK_HRESULT(hr);

#pragma endregion

#pragma region Depth Stencil State

  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
  depthStencilDesc.DepthEnable = true;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  hr = Device->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState);
  CHECK_HRESULT(hr);

  DeviceContext->OMSetDepthStencilState(DepthStencilState, 1);

#pragma endregion

#pragma region Depth Stencil View Desc

  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  hr = Device->CreateDepthStencilView(DepthStencilBuffer, &depthStencilViewDesc, &DepthStencilView);
  CHECK_HRESULT(hr);

  DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

#pragma endregion

#pragma region Rasterizer

  D3D11_RASTERIZER_DESC rasterDesc;
  rasterDesc.CullMode = GetGame()->initSettings.cullTriangles ? D3D11_CULL_BACK : D3D11_CULL_NONE;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = false;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable = false;
  rasterDesc.ScissorEnable = false;
  rasterDesc.SlopeScaledDepthBias = 0.0f;
  rasterDesc.AntialiasedLineEnable = true;

  hr = Device->CreateRasterizerState(&rasterDesc, &RasterState);
  CHECK_HRESULT(hr);

  DeviceContext->RSSetState(RasterState);

#pragma endregion

#pragma region Viewport

  D3D11_VIEWPORT viewport;
  viewport.Width = contextSize.x;
  viewport.Height = contextSize.y;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  DeviceContext->RSSetViewports(1, &viewport);

#pragma endregion

#pragma region Blend State

  D3D11_BLEND_DESC blendStateDesc;
  ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
  blendStateDesc.AlphaToCoverageEnable = FALSE;
  blendStateDesc.IndependentBlendEnable = FALSE;        
  blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
  blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  HRESULT result = Device->CreateBlendState(&blendStateDesc, &BlendState);
  CHECK_HRESULT(result);

  DeviceContext->OMSetBlendState(BlendState, nullptr, 0xFFFFFF);

#pragma endregion
}

// ----------------------------------------------------------------------------

void GraphicsDevice::CreateInputLayout(byte* bytecode,
                                       UINT bytecodeSize,
                                       D3D11_INPUT_ELEMENT_DESC* layoutDesc,
                                       UINT layoutDescNumElements,
                                       ID3D11InputLayout** layout)
{
  static D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
  {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  if (layoutDesc == nullptr)
  {
    layoutDesc = basicVertexLayoutDesc;
    layoutDescNumElements = ARRAYSIZE(basicVertexLayoutDesc);
  }

  Device->CreateInputLayout(
    layoutDesc,
    layoutDescNumElements,
    bytecode,
    bytecodeSize,
    layout);
}

// ----------------------------------------------------------------------------
