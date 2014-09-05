/*********************************
 * RenderSet.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "Helpers\FixedWindows.h"
#include "Event.h"
#include "EventHandlers.h"
#include <unordered_map>
#include <vector>

// ----------------------------------------------------------------------------

struct Shader;
struct Camera;

// ----------------------------------------------------------------------------

__interface Drawable
{
  void Draw();
};

// ----------------------------------------------------------------------------

class RenderSet
{
public:
  RenderSet(Camera *camera, const std::string& name, std::type_index camtype);

  void AddDrawable(Drawable *drawable, Shader *shader);
  void RemoveDrawable(Drawable *drawable);

  void Draw();

  IR_PROPERTY(Camera *, RenderCamera);
  IR_PROPERTY(std::string, Name);
  IR_PROPERTY(std::type_index, CameraType);

  struct DrawablePair
  {
    DrawablePair() = default;
    DrawablePair(Drawable *drawable, Shader *shader)
      : drawable(drawable), shader(shader)
    {
    }
    
    Drawable *drawable = nullptr;
    Shader *shader = nullptr;

    inline bool operator<(const DrawablePair& other)
    {
      return shader < other.shader;
    }
  };
  
private:
  std::vector<DrawablePair> drawables;
};

// ----------------------------------------------------------------------------

class RenderGroup : public Events::BasicClassEventReciever<RenderGroup>
{
public:
  template <typename CameraType>
  RenderSet *CreateSet(const std::string& name, CameraType *camera, bool perma);
  RenderSet *GetSet(const std::string& name);
  void RemoveSet(const std::string& name);
  void ClearSets();

  void Initialize();

  void Draw(Events::EventMessage&);

  static RenderGroup Instance;

private:
  RenderSet *CreateSet(const std::string& name, Camera *camera, 
                       std::type_index camtype, bool perma);
  RenderGroup();

  std::unordered_map<std::string, std::pair<RenderSet *, bool>> sets;
};

// ----------------------------------------------------------------------------

template <typename CameraType>
RenderSet *RenderGroup::CreateSet(const std::string& name, 
                                  CameraType *camera, bool perma)
{
  return CreateSet(name, camera, typeid(CameraType), perma);
}

// ----------------------------------------------------------------------------

