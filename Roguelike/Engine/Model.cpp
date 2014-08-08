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
             ID3D11Buffer *indexBuffer, UINT indexCount, UINT stride)
             : _vertexBuffer(vertexBuffer), _vertexCount(vertexCount),
               _indexBuffer(indexBuffer), _indexCount(indexCount), _stride(stride)
{
}

void XM_CALLCONV Model::draw(DirectX::FXMMATRIX worldTransform) const
{
  shader->camera->worldMatrix = worldTransform;

  unsigned offset = 0;

  auto context = shader->device->deviceContext();
  context->IASetVertexBuffers(0, 1, &_vertexBuffer, &_stride, &offset);
  context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, offset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  shader->draw(_indexCount);
}

