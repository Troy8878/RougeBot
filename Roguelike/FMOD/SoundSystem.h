/*********************************
* SoundSystem.h
* Troy
* Created 2014/09/26
*********************************/

#pragma once

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include "Helpers/FixedWindows.h"
#include <vector>

class SoundClass
{

public:
  SoundClass();
  ~SoundClass();
  void Update();

  class Sound
  {
  public:
    enum class EXTRA_INFO;
    enum class SOUND_TYPE;

    enum class EXTRA_INFO
    {
      NONE,              // No special info! (default)
      LENGTH,            // Number of bytes to load (if not all of them)
      FILE_OFFSET,       // Where to start loading (if not the file's beginning)
      NUM_CHANNELS,      // Channels being used (RAW SOUND DATA ONLY)
      CUSTOM_FREQUENCY,  // If the sound needs to be played at a frequency other than the value of hertz
      FORMAT,            // File format to cast the file as (RAW SOUND DATA ONLY)
      STREAM_BUFFER,     // Size of a stream's buffer (STREAMS ONLY)
      INIT_SUBSOUND,     // Specify a sample within the sound to play
      NUM_SUBSOUNDS,     // Limits the number of subsounds in the sound
      SUBSOUND_LIST,     // Give an array of subsounds for this sound to use
      SBSND_LIST_NUM,    // The number of elements in SUBSOUND_LIST (REQUIRED WITH SUBSOUND_LIST!)
      PCM_RW_CALLBACK,   // Callback to read/write PCM data as the sound is being opened
      PCM_SEEK_CALLBACK, // Callback for seeking functions in multi-sample sounds
      NB_CALLBACK,       // Callback for FMOD::NONBLOCKING (success or error)
      DLS_SET,           // Get a DLS file to use (MIDI ONLY)
      FSB_ENC_KEY,       // For encrypted .fsb files
      MAX_POLYPHONY,     // Change the max voice count on sequenced sound files
      USER_DATA,         // Apply user data to the sound file
      USE_THIS_CODEC,    // Specify the codec to speed up loading

      // USER DATA CALLBACKS!!
      USER_DATA_OPEN,    // Open user data
      USER_DATA_CLOSE,   // Close user data
      USER_DATA_READ,    // Read user data
      USER_DATA_SEEK,    // Seek user data
      USER_DATA_SR,      // Seek & read user data 
      USER_DATA_SC,      // Seek & cancel user data
      USER_DATA_TO_CALL, // User data to call
      // END DATA CALLBACKS

      CHANNEL_ORDER,     // Manually set the mapping of multi-channel sounds (more than one Sound class in a sound) to speakers
      CHANNEL_MASK,      // Manually set the mapping of normal sounds to speakers
      INIT_SOUND_GROUP,  // Put this sound into the given sound group
      INIT_SEEK,         // Set a position to seek a sound stream to
      INIT_SEEK_TIME,    // (Use with INIT_SEEK!).  This is the time the seek starts at
      IGNORE_SET_FILE,   // Have FMOD manage file usage instead of code calling setFileSystem
      AUDIO_QUEUE,       // Determine whether hardware or software decodes the sound manually
      MIDI_GRANULARITY,  // Default is 512.  Lower = greater accuracy, higher = less CPU usage (MIDI ONLY)
      NON_BLOCK_THREAD,  // Specify threads to load with (non-blocked).  4 is the highest value allowed!
    };

    enum class SOUND_TYPE
    {
      NONE,
      MUSIC,
      SFX  // AKA sound effects, but let's not confuse that term with every other term in here with the word "sound" in it
    };

    struct ExInfo
    {

      union INFO
      {
        int exInfoInt;
        unsigned int exInfoU_Int;
        int *exInfoIP;
        void *exInfoVP;
        const char *exInfoCChar;
        FMOD_SOUND_FORMAT exInfoFSF;
        FMOD_SOUND_PCMREAD_CALLBACK exInfoPCMRW;
        FMOD_SOUND_PCMSETPOS_CALLBACK exInfoPCMSeek;
        FMOD_SOUND_NONBLOCK_CALLBACK exInfoNonBlock;
        FMOD_SOUND_TYPE exInfoSoundType;
        FMOD_FILE_OPEN_CALLBACK exInfoFOpen;
        FMOD_FILE_CLOSE_CALLBACK exInfoFClose;
        FMOD_FILE_READ_CALLBACK exInfoFRead;
        FMOD_FILE_SEEK_CALLBACK exInfoFSeek;
        FMOD_FILE_ASYNCREAD_CALLBACK exInfoARead;
        FMOD_CHANNELORDER exInfoChanOrder;
        FMOD_SOUNDGROUP *exInfoSoundGroup;
      };

      EXTRA_INFO type = EXTRA_INFO::NONE;
      INFO u_info;  // Regarded as having no info until type is set to something other than none
    };

    Sound(const char* name, SoundClass Sys, SOUND_TYPE type = SOUND_TYPE::NONE, FMOD_MODE custom = 0, std::vector<ExInfo> Infos = {});  // type = MUSIC (0) for looping or SFX (1) for non-looping.  Use the custom constructors instead!
    ~Sound();
    Sound CreateSound(const char* name, SoundClass Sys, std::vector<ExInfo> Infos = {});  // See the EXTRA_INFO in SoundSystem.h if there is extra info.
    Sound CreateMusic(const char* name, SoundClass Sys, std::vector<ExInfo> Infos = {});  // See the EXTRA_INFO in SoundSystem.h if there is extra info.
    Sound CreateCustom(const char* name, SoundClass Sys, FMOD_MODE custom, std::vector<ExInfo> Infos = {});  // For if the sound isn't supposed to just loop or play once.
    std::vector<ExInfo> GetExInfo() const;
    void Play();
    bool IsPlaying();
    void Pause();
    void Unpause();
    bool IsPaused();
    void Stop();
    int GetCurrentChan();

  private:
    std::vector <ExInfo> Infos = {};
    FMOD::Sound *sound;
    FMOD::Channel *chan;
    SoundClass *Sys;
    FMOD_RESULT FMODresult;
    void CheckResult(bool isOkay);
  };

  friend class Sound;

private:
  void CheckResult(bool isOkay);
  FMOD::System *SoundSystem;
  FMOD_RESULT FMODresult;
  FMOD_SPEAKERMODE speakermode;
  unsigned int version;
  int hertz = 44100;
};