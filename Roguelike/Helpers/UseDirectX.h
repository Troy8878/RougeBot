/*********************************
 * UseDirectX.h
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#pragma once

#include "FixedWindows.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <d2d1_2.h>
#include <dxgi1_2.h>

typedef DirectX::XMVECTOR XVector;

#include "Helpers\VectorHelp.h"

#define ARRAY_STRIDE(arr) ((UINT)(reinterpret_cast<BYTE *>((arr) + 1) - reinterpret_cast<BYTE *>(arr)))

template <typename To, typename From, typename Holder>
HRESULT ConvertInterface(From *current, Holder **result)
{
  return current->QueryInterface(__uuidof(To), reinterpret_cast<void **>(result));
}
