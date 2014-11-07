/*********************************
 * SoundManager.h
 * Connor Hilarides
 * Created 2014/10/24
 *********************************/

#pragma once

#include "SoundSystem.h"

class ManagedSound
{
public:
  ManagedSound() = default;

  ManagedSound(ManagedSound &&managed) : _sound(std::move(managed._sound))
  {
  }

  SoundClass::Sound *operator->();
  SoundClass::Sound *get();

private:
  ManagedSound(SoundClass::Sound *sound);
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