/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

