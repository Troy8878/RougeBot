/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
 *********************************/

#pragma once

class WorldSnapshot;
class Entity;

__interface AIBehaviour
{
  void ApplyBehaviour(const WorldSnapshot& world);
  void InitializeTarget(Entity *target);
  void InitilizeOwner(Entity *thisEntity);

};

