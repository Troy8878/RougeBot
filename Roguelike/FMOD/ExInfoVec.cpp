/*********************************
 * ExInfoVec.cpp
 * Troy
 * Created 2014/10/9
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, int data, std::vector<SoundClass::Sound::ExInfo> &vec)
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
  throw basic_exception("Attempted to add a non-int to an int type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, unsigned int data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::LENGTH
    || info == SoundClass::Sound::EXTRA_INFO::FILE_OFFSET
    || info == SoundClass::Sound::EXTRA_INFO::STREAM_BUFFER
    || info == SoundClass::Sound::EXTRA_INFO::INIT_SEEK
    || info == SoundClass::Sound::EXTRA_INFO::AUDIO_QUEUE
    || info == SoundClass::Sound::EXTRA_INFO::MIDI_GRANULARITY
    || info == SoundClass::Sound::EXTRA_INFO::CHANNEL_MASK
    || info == SoundClass::Sound::EXTRA_INFO::INIT_SEEK_TIME)
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
  throw basic_exception("Attempted to add a non-uint to an uint type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, int *data, std::vector<SoundClass::Sound::ExInfo> &vec)
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
  throw basic_exception("Attempted to add a non-int pointer to an int pointer type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, void *data, std::vector<SoundClass::Sound::ExInfo> &vec)
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
  throw basic_exception("Attempted to add a non-void pointer to a void pointer type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, const char *data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::DLS_SET
    || info == SoundClass::Sound::EXTRA_INFO::FSB_ENC_KEY)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoCChar = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoCChar = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add a not const char pointer to a const char pointer type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_FORMAT data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::FORMAT)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoFSF = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoFSF = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add a non-sound format to a sound format type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_PCMREAD_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::PCM_RW_CALLBACK)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoPCMRW = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoPCMRW = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add an invalid PCM Read/Write Callback!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_PCMSETPOS_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::PCM_SEEK_CALLBACK)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoPCMSeek = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoPCMSeek = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add an invalid PCM Seek Callback!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_NONBLOCK_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::NB_CALLBACK)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoNonBlock = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoNonBlock = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add an invalid Non-Block Callback!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_TYPE data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USE_THIS_CODEC)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoSoundType = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoSoundType = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add an invalid Sound Type suggestion!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_OPEN_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USER_DATA_OPEN)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoFOpen = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoFOpen = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Use USER_DATA_OPEN for opening custom files!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_CLOSE_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USER_DATA_CLOSE)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoFClose = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoFClose = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Use USER_DATA_CLOSE for closing custom files!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_READ_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USER_DATA_READ)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoFRead = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoFRead = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Use USER_DATA_READ for reading custom files!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_SEEK_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USER_DATA_SEEK)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoFSeek = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoFSeek = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Use USER_DATA_SEEK for seeking custom files!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_ASYNCREAD_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::USER_DATA_SR
    || info == SoundClass::Sound::EXTRA_INFO::USER_DATA_SC)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoARead = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoARead = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Use USER_DATA_SR for reading asynchronous custom files or USER_DATA_SC for cancelling them!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_CHANNELORDER data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::CHANNEL_ORDER)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoChanOrder = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoChanOrder = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Attempted to add a non-channel order to a channel order type!");
}

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUNDGROUP *data, std::vector<SoundClass::Sound::ExInfo> &vec)
{
  // Only assign this if it's one of the following: (otherwise it's the wrong type)
  if (info == SoundClass::Sound::EXTRA_INFO::INIT_SOUND_GROUP)
  {
    // Overwrite data if it's already in place
    std::vector<SoundClass::Sound::ExInfo>::iterator it = FindExInfo(info, vec);
    if (it != vec.end())
    {
      it->u_info.exInfoSoundGroup = data;
    }

    // otherwise just add in as new ExInfo
    else
    {
      SoundClass::Sound::ExInfo madeInfo;
      madeInfo.type = info;
      madeInfo.u_info.exInfoSoundGroup = data;
      vec.push_back(madeInfo);
    }

    // Return here!
    return;
  }

  // If this is reached, we have a problem!
  throw basic_exception("Use INIT_SOUND_GROUP to set this sound for a sound group!");
}