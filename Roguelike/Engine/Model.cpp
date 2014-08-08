/*********************************
 * Model.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#include "Common.h"
#include "Model.h"
#include "Shader.h"

static BasicVertex *BasicVertexPtr;

Model::Model(ID3D11Buffer *vertexBuffer, UINT vertexCount, 
             ID3D11Buffer *indexBuffer, UINT indexCount)
             : _vertexBuffer(vertexBuffer), _vertexCount(vertexCount),
               _indexBuffer(indexBuffer), _indexCount(indexCount)
{
}

void XM_CALLCONV Model::draw(DirectX::FXMMATRIX worldTransform) const
{
  _shader->camera->worldMatrix = worldTransform;

  unsigned stride = ARRAY_STRIDE(BasicVertexPtr);
  unsigned offset = 0;

  auto context = _shader->device->deviceContext();
  context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
  context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, offset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  _shader->draw(_indexCount);
}

