/*********************************
 * CameraComponent.h
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#pragma once

#include "Common.h"
#include "Camera.h"

#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class CameraFactoryComponent;

// ----------------------------------------------------------------------------

class CameraComponent : public Component
{
public:
  CameraComponent(const std::string& name, int layer, const MultiCam& camera);
  ~CameraComponent();


};
