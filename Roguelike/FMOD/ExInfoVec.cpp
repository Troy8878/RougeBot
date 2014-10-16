/*********************************
* ExInfoVec.cpp
* Troy
* Created 2014/10/9
*********************************/

#include "ExInfoVec.h"

std::vector<SoundClass::Sound::ExInfo>::iterator FindExInfo(SoundClass::Sound::EXTRA_INFO info, std::vector<SoundClass::Sound::ExInfo> vec)
{
  // Don't search from an empty vector
  if (vec.empty())
  {
    return vec.end();
  }

  std::vector<SoundClass::Sound::ExInfo>::iterator iter = vec.begin();

  // Otherwise search for the given type
  for (; iter != vec.end(); ++iter)
  {
    // We've found the requested info if this is true
    if (iter->type == info)
    {
      return iter;
    }
  }

  // If nothing is found, return the iter at end.  The function that called this should deal with it on its own
  return vec.end();

}

void DeleteExInfo(SoundClass::Sound::EXTRA_INFO info, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Don't delete from an empty vector
  if (vec.empty())
  {
    return;
  }

  // Otherwise search for the given type
  std::vector<SoundClass::Sound::ExInfo>::iterator iter = FindExInfo(info, vec);

  // We've found it if iter does not equal our NULL substitute
  if (iter != vec.end())
  {
    iter = vec.erase(iter);
  }

}

template <class T>
void AddExInfo(SoundClass::Sound::EXTRA_INFO info, T data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // If this is reached, then the data type of T is not supported!
  throw std::exception("Attempted to add invalid ExInfo type!");
}

template <>
void AddExInfo<int>(SoundClass::Sound::EXTRA_INFO info, int data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::NUM_CHANNELS
    || info == SoundClass::Sound::EXTRA_INFO::CUSTOM_FREQUENCY
    || info == SoundClass::Sound::EXTRA_INFO::INIT_SUBSOUND
    || info == SoundClass::Sound::EXTRA_INFO::NUM_SUBSOUNDS
    || info == SoundClass::Sound::EXTRA_INFO::SBSND_LIST_NUM
    || info == SoundClass::Sound::EXTRA_INFO::MAX_POLYPHONY
    || info == SoundClass::Sound::EXTRA_INFO::IGNORE_SET_FILE)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoInt = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoInt = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw std::exception("Attempted to add a non-int to an int type!");
}

template <>
void AddExInfo<unsigned int>(SoundClass::Sound::EXTRA_INFO info, unsigned int data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::LENGTH
    || info == SoundClass::Sound::EXTRA_INFO::FILE_OFFSET
    || info == SoundClass::Sound::EXTRA_INFO::STREAM_BUFFER
    || info == SoundClass::Sound::EXTRA_INFO::INIT_SEEK
    || info == SoundClass::Sound::EXTRA_INFO::AUDIO_QUEUE
    || info == SoundClass::Sound::EXTRA_INFO::MIDI_GRANULARITY)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoU_Int = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoU_Int = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw std::exception("Attempted to add a non-uint to an uint type!");
}

template <>
void AddExInfo<int*>(SoundClass::Sound::EXTRA_INFO info, int *data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::SUBSOUND_LIST)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoIP = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoIP = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw std::exception("Attempted to add a non-int pointer to an int pointer type!");
}

template <>
void AddExInfo<void*>(SoundClass::Sound::EXTRA_INFO info, void *data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USER_DATA
    || info == SoundClass::Sound::EXTRA_INFO::USER_DATA_TO_CALL)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoVP = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoVP = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw std::exception("Attempted to add a non-void pointer to a void pointer type!");
}

template <>
void AddExInfo<const char*>(SoundClass::Sound::EXTRA_INFO info, const char *data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_SOUND_FORMAT>(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_FORMAT data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_SOUND_PCMREAD_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_PCMREAD_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_SOUND_PCMSETPOS_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_PCMSETPOS_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_SOUND_NONBLOCK_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_NONBLOCK_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_SOUND_TYPE>(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_TYPE data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_FILE_OPEN_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_OPEN_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_FILE_CLOSE_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_CLOSE_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_FILE_READ_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_READ_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_FILE_SEEK_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_SEEK_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_FILE_ASYNCREAD_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_ASYNCREAD_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_FILE_ASYNCCANCEL_CALLBACK>(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_ASYNCCANCEL_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_CHANNELORDER>(SoundClass::Sound::EXTRA_INFO info, FMOD_CHANNELORDER data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_CHANNELMASK>(SoundClass::Sound::EXTRA_INFO info, FMOD_CHANNELMASK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_SOUNDGROUP*>(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUNDGROUP* data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}

template <>
void AddExInfo<FMOD_TIMEUNIT>(SoundClass::Sound::EXTRA_INFO info, FMOD_TIMEUNIT data, std::vector<SoundClass::Sound::ExInfo> &vec)
{

}