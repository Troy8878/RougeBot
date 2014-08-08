/*********************************
 * Texture.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "Common.h"
#include "Texture.h"

Texture::Texture(ID3D11Device *device, const fs::wpath& file)
{
  ID3D11ShaderResourceView *res;

  D3D11_TEXTURE2D_DESC desc;
  
  (device, file, res, desc);
}

