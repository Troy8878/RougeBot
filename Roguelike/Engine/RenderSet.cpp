/*********************************
 * RenderSet.cpp
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#include "Common.h"
#include "RenderSet.h"
#include "Shader.h"
#include "Camera.h"

// ----------------------------------------------------------------------------

RenderSet::RenderSet(ICamera *camera, const std::string &name,
                     int priority, std::type_index camtype)
  : _RenderCamera(camera), _Name(name), _CameraType(camtype), _Priority(priority)
{
}

// ----------------------------------------------------------------------------

void RenderSet::AddDrawable(Drawable *drawable, Shader *shader)
{
  drawables.push_back({drawable, shader});
  std::sort(drawables.begin(), drawables.end());
}

// ----------------------------------------------------------------------------

static bool operator==(const RenderSet::DrawablePair &dp, Drawable *dr)
{
  return dp.drawable == dr;
}

void RenderSet::RemoveDrawable(Drawable *drawable)
{
  auto it = std::find(drawables.begin(), drawables.end(), drawable);
  if (it != drawables.end())
    drawables.erase(it);
}

// ----------------------------------------------------------------------------

void RenderSet::Draw()
{
  for (auto &pair : drawables)
  {
    pair.shader->camera = RenderCamera;
    pair.drawable->Draw();
  }
}

// ----------------------------------------------------------------------------

RenderGroup RenderGroup::Instance;

// ----------------------------------------------------------------------------

RenderGroup::RenderGroup()
  : BasicClassEventReciever(this)
{
}

// ----------------------------------------------------------------------------

void RenderGroup::Initialize()
{
  static Events::EventId drawId("draw");
  SetHandler(drawId, &RenderGroup::Draw);
}

// ----------------------------------------------------------------------------

RenderSet *RenderGroup::GetSet(const std::string &name)
{
  auto it = sets.find(name);
  return it != sets.end() ? it->second.first : nullptr;
}

// ----------------------------------------------------------------------------

RenderSet *RenderGroup::CreateSet(const std::string &name, ICamera *camera,
                                  std::type_index camtype, int pri, bool perma)
{
  std::pair<RenderSet *, bool> pair{new RenderSet(camera, name, pri, camtype), perma};
  sets[name] = pair;

  UpdatePriorities();

  return pair.first;
}

// ----------------------------------------------------------------------------

void RenderGroup::RemoveSet(const std::string &name)
{
  auto set = sets[name].first;
  for (auto &drawable : set->drawables)
  {
    drawable.drawable->OnSetDestroyed();
  }

  delete set;
  sets.erase(name);

  UpdatePriorities();
}

// ----------------------------------------------------------------------------

void RenderGroup::ClearSets()
{
  for (auto &pair : sets)
  {
    if (pair.second.second)
      continue;

    RemoveSet(pair.first);
    ClearSets();
    return;
  }
}

// ----------------------------------------------------------------------------

void RenderGroup::Draw(Events::EventMessage &)
{
  for (auto &set : priorityList)
  {
    set.second->Draw();
  }
}

// ----------------------------------------------------------------------------

void RenderGroup::UpdatePriorities()
{
  priorityList.clear();
  for (auto &set : sets)
  {
    priorityList.push_back({set.second.first->Priority, set.second.first});
  }

  std::sort(priorityList.begin(), priorityList.end());
}

// ----------------------------------------------------------------------------