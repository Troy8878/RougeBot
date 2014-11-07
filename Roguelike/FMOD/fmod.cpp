/*********************************
* fmod.cpp
* Troy
* Created 2014/09/26
*********************************/

#include "SoundSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// ----------------------------------------------------------------------------

/* Here be the SoundClass that manages the playing of sounds! */

SoundClass::SoundClass()
{
}

// ----------------------------------------------------------------------------

void SoundClass::Initialize()
{
  // Create our system
  FMODresult = FMOD::System_Create(&SoundSystem);
  CheckResult(FMODresult == FMOD_OK);

  // Ensure that it's the right version
  FMODresult = SoundSystem->getVersion(&version);
  CheckResult(FMODresult == FMOD_OK);

  if (version < FMOD_VERSION)
  {
    std::cout << "FMOD lib version " << version << " doesn't match header version " << FMOD_VERSION;
  }

  // Set the user selected speaker mode if it was given.  Default to stereo & 44100 Hz
  if (speakermode)
  {
    FMODresult = SoundSystem->setSoftwareFormat(hertz, speakermode, 2);
  }
  else
  {
    FMODresult = SoundSystem->setSoftwareFormat(hertz, FMOD_SPEAKERMODE_DEFAULT, 2);
  }
  CheckResult(FMODresult == FMOD_OK);

  // Initialize FMOD
  FMODresult = SoundSystem->init(512, FMOD_INIT_NORMAL, nullptr);
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

SoundClass::~SoundClass()
{
  FMODresult = SoundSystem->release();
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

void SoundClass::Update()
{
  FMODresult = SoundSystem->update();
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

void SoundClass::CheckResult(bool isOkay)
{
  if (!isOkay)
  {
    auto err_str = FMOD_ErrorString(FMODresult);
    std::cout << "FMOD error " << FMODresult << ":  " << err_str;
    throw basic_exception("FMOD init error.");
  }
}

// ----------------------------------------------------------------------------

/* Here be the sound class itself! */

SoundClass::Sound::Sound(const char *name, SoundClass &Sys, SOUND_TYPE type, FMOD_MODE custom, std::vector<ExInfo> Infos)
{
  FMOD_MODE mode;
  FMOD_CREATESOUNDEXINFO ex = {0};

  if (custom)
  {
    mode = custom;
  }

  else if (type == SOUND_TYPE::MUSIC)
  {
    mode = FMOD_LOOP_NORMAL ;
  }

  else // If type & custom are both zero, we make a (non-looping) sound
  {
    mode = FMOD_LOOP_OFF ;
  }

  if (!Infos.empty())
  {
    for (std::vector<ExInfo>::iterator iter = Infos.begin(); iter != Infos.end(); ++iter)
    {
      // This works because each data type is matched to its union type
      switch (iter->type)
      {
        case EXTRA_INFO::LENGTH:
          ex.length = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::FILE_OFFSET:
          ex.fileoffset = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::NUM_CHANNELS:
          ex.numchannels = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::CUSTOM_FREQUENCY:
          ex.defaultfrequency = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::FORMAT:
          ex.format = iter->u_info.exInfoFSF;
          break;
        case EXTRA_INFO::STREAM_BUFFER:
          ex.decodebuffersize = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::INIT_SUBSOUND:
          ex.initialsubsound = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::NUM_SUBSOUNDS:
          ex.numsubsounds = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::SUBSOUND_LIST:
          ex.inclusionlist = iter->u_info.exInfoIP;
          break;
        case EXTRA_INFO::SBSND_LIST_NUM:
          ex.inclusionlistnum = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::PCM_RW_CALLBACK:
          ex.pcmreadcallback = iter->u_info.exInfoPCMRW;
          break;
        case EXTRA_INFO::PCM_SEEK_CALLBACK:
          ex.pcmsetposcallback = iter->u_info.exInfoPCMSeek;
          break;
        case EXTRA_INFO::NB_CALLBACK:
          ex.nonblockcallback = iter->u_info.exInfoNonBlock;
          break;
        case EXTRA_INFO::DLS_SET:
          ex.dlsname = iter->u_info.exInfoCChar;
          break;
        case EXTRA_INFO::FSB_ENC_KEY:
          ex.encryptionkey = iter->u_info.exInfoCChar;
          break;
        case EXTRA_INFO::MAX_POLYPHONY:
          ex.maxpolyphony = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::USER_DATA:
          ex.userdata = iter->u_info.exInfoVP;
          break;
        case EXTRA_INFO::USE_THIS_CODEC:
          ex.suggestedsoundtype = iter->u_info.exInfoSoundType;
          break;
        case EXTRA_INFO::USER_DATA_OPEN:
          ex.fileuseropen = iter->u_info.exInfoFOpen;
          break;
        case EXTRA_INFO::USER_DATA_CLOSE:
          ex.fileuserclose = iter->u_info.exInfoFClose;
          break;
        case EXTRA_INFO::USER_DATA_READ:
          ex.fileuserread = iter->u_info.exInfoFRead;
          break;
        case EXTRA_INFO::USER_DATA_SEEK:
          ex.fileuserseek = iter->u_info.exInfoFSeek;
          break;
        case EXTRA_INFO::USER_DATA_SR:
          ex.fileuserasyncread = iter->u_info.exInfoARead;
          break;
        case EXTRA_INFO::USER_DATA_SC:
          ex.fileuserasynccancel = iter->u_info.exInfoARead;
          break;
        case EXTRA_INFO::USER_DATA_TO_CALL:
          ex.fileuserdata = iter->u_info.exInfoVP;
          break;
        case EXTRA_INFO::CHANNEL_ORDER:
          ex.channelorder = iter->u_info.exInfoChanOrder;
          break;
        case EXTRA_INFO::CHANNEL_MASK:
          ex.channelmask = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::INIT_SOUND_GROUP:
          ex.initialsoundgroup = iter->u_info.exInfoSoundGroup;
          break;
        case EXTRA_INFO::INIT_SEEK:
          ex.initialseekposition = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::INIT_SEEK_TIME:
          ex.initialseekpostype = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::IGNORE_SET_FILE:
          ex.ignoresetfilesystem = iter->u_info.exInfoInt;
          break;
        case EXTRA_INFO::AUDIO_QUEUE:
          ex.audioqueuepolicy = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::MIDI_GRANULARITY:
          ex.minmidigranularity = iter->u_info.exInfoU_Int;
          break;
        case EXTRA_INFO::NON_BLOCK_THREAD:
          ex.nonblockthreadid = iter->u_info.exInfoInt;
          break;
        default:
          break;
      }
    }

    FMODresult = Sys.SoundSystem->createSound(name, mode, &ex, &sound);
    CheckResult(FMODresult == FMOD_OK);
  }
  else
  {
    FMODresult = Sys.SoundSystem->createSound(name, mode, nullptr, &sound);
    CheckResult(FMODresult == FMOD_OK);
  }

  // Save the Sys for later
  this->Sys = &Sys;

  // chan is set when we play the sound (that's when a channel is alocated)
}

// ----------------------------------------------------------------------------

SoundClass::Sound *SoundClass::Sound::CreateSound(const char *name, SoundClass &Sys, std::vector<ExInfo> Infos)
{
  // Just a quick wrapper for easy SFX construction
  return new Sound(name, Sys, SOUND_TYPE::SFX, 0, Infos);
}

// ----------------------------------------------------------------------------

SoundClass::Sound *SoundClass::Sound::CreateMusic(const char *name, SoundClass &Sys, std::vector<ExInfo> Infos)
{
  // Just a quick wrapper for easy music construction
  return new Sound(name, Sys, SOUND_TYPE::MUSIC, 0, Infos);
}

// ----------------------------------------------------------------------------

SoundClass::Sound *SoundClass::Sound::CreateCustom(const char *name, SoundClass &Sys, FMOD_MODE custom, std::vector<ExInfo> Infos)
{
  // In case we need easy construction for something other than standard SFX or music
  return new Sound(name, Sys, SOUND_TYPE::NONE, custom, Infos);
}

// ----------------------------------------------------------------------------

std::vector<SoundClass::Sound::ExInfo> SoundClass::Sound::GetExInfo() const
{
  // Just return a non-rewritable version of Infos
  return Infos;
}

// ----------------------------------------------------------------------------

void SoundClass::Sound::Play()
{
  FMODresult = Sys->SoundSystem->playSound(sound, nullptr, false, &chan);
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

bool SoundClass::Sound::IsPlaying()
{
  bool answer;
  FMODresult = chan->isPlaying(&answer);
  CheckResult(FMODresult == FMOD_OK);
  return answer;
}

// ----------------------------------------------------------------------------

void SoundClass::Sound::Pause()
{
  FMODresult = chan->setPaused(true);
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

void SoundClass::Sound::Unpause()
{
  FMODresult = chan->setPaused(false);
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

bool SoundClass::Sound::IsPaused()
{
  bool answer;
  FMODresult = chan->getPaused(&answer);
  CheckResult(FMODresult == FMOD_OK);
  return answer;
}

// ----------------------------------------------------------------------------

void SoundClass::Sound::Stop()
{
  // Stopping is by channel, not by sound (the channel is what plays, while the sound is what the channel is playing!)
  FMODresult = chan->stop();
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

int SoundClass::Sound::GetCurrentChan()
{
  int answer;
  FMODresult = chan->getIndex(&answer);
  CheckResult(FMODresult == FMOD_OK);
  return answer;
}

// ----------------------------------------------------------------------------

SoundClass::Sound::~Sound()
{
  FMODresult = sound->release();
  CheckResult(FMODresult == FMOD_OK);
}

// ----------------------------------------------------------------------------

void SoundClass::Sound::CheckResult(bool isOkay)
{
  if (!isOkay)
  {
    std::cout << "FMOD error " << FMODresult << ":  " << FMOD_ErrorString(FMODresult);
    throw basic_exception("FMOD init error.");
  }
}

// ----------------------------------------------------------------------------