/*********************************
 * SoundManager.h
 * Connor Hilarides
 * Created 2014/10/24
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "SoundSystem.h"

class ManagedSound
{
public:
  ManagedSound() = default;
  ManagedSound(const ManagedSound &managed) = default;

  SoundClass::Sound *operator->();
  SoundClass::Sound *get();

private:
  explicit ManagedSound(SoundClass::Sound *sound);
  friend class SoundManager;

  std::shared_ptr<SoundClass::Sound> _sound;
};

class SoundManager
{
public:
  static SoundManager Instance;

  ManagedSound Load(json::value definition);

private:
  ManagedSound LoadAsset(const std::string &asset);
  ManagedSound LoadDefinition(json::value definition);

  std::unordered_map<std::string, std::weak_ptr<SoundClass::Sound>> cache;
};