/*********************************
 * UseDirectX.h
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#pragma once

#include "FixedWindows.h"
#include <DirectXMath.h>
#include <d3d11.h>

typedef DirectX::XMVECTOR XVector;

#include "Helpers\VectorHelp.h"

#define ARRAY_STRIDE(arr) ((UINT)(reinterpret_cast<BYTE *>((arr) + 1) - reinterpret_cast<BYTE *>(arr)))
