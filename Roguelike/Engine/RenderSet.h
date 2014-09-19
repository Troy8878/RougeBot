/*********************************
 * RenderSet.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "Common.h"
#include "EventHandlers.h"

// ----------------------------------------------------------------------------

struct Shader;
struct Camera;
struct ICamera;

// ----------------------------------------------------------------------------

__interface Drawable
{
  void Draw();
  void OnSetDestroyed();
};

// ----------------------------------------------------------------------------

class RenderSet
{
public:
  RenderSet(ICamera *camera, const std::string& name, int priority, std::type_index camtype);

  void AddDrawable(Drawable *drawable, Shader *shader);
  void RemoveDrawable(Drawable *drawable);

  void Draw();

  IR_PROPERTY(ICamera *, RenderCamera);
  IR_PROPERTY(std::string, Name);
  IR_PROPERTY(std::type_index, CameraType);
  IR_PROPERTY(int, Priority);

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

  friend class RenderGroup;
};

// ----------------------------------------------------------------------------

class RenderGroup : public Events::BasicClassEventReciever<RenderGroup>
{
public:
  template <typename CameraType>
  RenderSet *CreateSet(const std::string& name, CameraType *cam, int priority, bool perma);
  RenderSet *GetSet(const std::string& name);
  void RemoveSet(const std::string& name);
  void ClearSets();

  void Initialize();

  void Draw(Events::EventMessage&);

  static RenderGroup Instance;

private:
  RenderSet *CreateSet(const std::string& name, ICamera *camera, 
                       std::type_index camtype, int pri, bool perma);
  RenderGroup();

  void UpdatePriorities();

  std::unordered_map<std::string, std::pair<RenderSet *, bool>> sets;
  std::vector<std::pair<int, RenderSet *>> priorityList;
};

// ----------------------------------------------------------------------------

template <typename CameraType>
RenderSet *RenderGroup::CreateSet(const std::string& name,
                                  CameraType *cam, int priority, bool perma)
{
  return CreateSet(name, cam, typeid(CameraType), priority, perma);
}

// ----------------------------------------------------------------------------

