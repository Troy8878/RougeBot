/*********************************
 * SoundManager.cpp
 * Connor Hilarides
 * Created 2014/10/24
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "SoundManager.h"
#include "Engine/Common.h"

// ----------------------------------------------------------------------------

SoundClass::Sound *ManagedSound::operator->()
{
  return _sound.get();
}

// ----------------------------------------------------------------------------

SoundClass::Sound *ManagedSound::get()
{
  return _sound.get();
}

// ----------------------------------------------------------------------------

ManagedSound::ManagedSound(SoundClass::Sound *sound)
  : _sound(sound)
{
}

// ----------------------------------------------------------------------------

SoundManager SoundManager::Instance;

// ----------------------------------------------------------------------------

ManagedSound SoundManager::Load(json::value definition)
{
  if (definition.is(json::json_type::jstring))
  {
    return LoadAsset(definition.as_string());
  }
  if (definition.is(json::json_type::jobject))
  {
    return LoadDefinition(definition);
  }

  throw string_exception("Could not load audio definition " + definition.serialize());
}

// ----------------------------------------------------------------------------

ManagedSound SoundManager::LoadAsset(const std::string &asset)
{
  return Load(ParseJsonAsset("Audio", asset + ".audiodef"));
}

// ----------------------------------------------------------------------------

ManagedSound SoundManager::LoadDefinition(json::value definition)
{
  auto &obj = definition.as_object();
  decltype(obj.cbegin()) it;

  auto *audio = GetGame()->Respack["Audio"];
  RELEASE_AFTER_SCOPE(audio);

  // TODO: Make some ExInfos
  std::vector<SoundClass::Sound::ExInfo> infos;

  if (map_get_check(obj, it, "sound"))
  {
    json::value filename = it->second;

    auto *soundFile = audio->GetResource(filename.as_string());
    RELEASE_AFTER_SCOPE(audio);

    auto tempFile = soundFile->GetTempFile();
    auto tempFilename = narrow(tempFile.getPath().file_string());

    auto sound = SoundClass::Sound::CreateSound(tempFilename.c_str(), GetGame()->SoundSystem, infos);
    return ManagedSound(sound);
  }
  if (map_get_check(obj, it, "music"))
  {
    json::value filename = it->second;

    auto *soundFile = audio->GetResource(filename.as_string());
    RELEASE_AFTER_SCOPE(audio);

    auto tempFile = soundFile->GetTempFile();
    auto tempFilename = narrow(tempFile.getPath().file_string());

    auto sound = SoundClass::Sound::CreateMusic(tempFilename.c_str(), GetGame()->SoundSystem, infos);
    return ManagedSound(sound);
  }

  throw basic_exception("Sound definitions must contain either a 'sound' or 'music' field.");
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------