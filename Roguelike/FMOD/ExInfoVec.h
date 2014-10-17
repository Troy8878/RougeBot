/*********************************
* ExInfoVec.h
* Troy
* Created 2014/10/9
*********************************/

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include "SoundSystem.h"
#include <vector>

std::vector<SoundClass::Sound::ExInfo>::iterator FindExInfo(SoundClass::Sound::EXTRA_INFO info, std::vector<SoundClass::Sound::ExInfo> vec);

void DeleteExInfo(SoundClass::Sound::EXTRA_INFO info, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, int data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, unsigned int data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, int *data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, void *data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, const char *data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_FORMAT data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_PCMREAD_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_PCMSETPOS_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_NONBLOCK_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUND_TYPE data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_OPEN_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_CLOSE_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_READ_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_SEEK_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_FILE_ASYNCREAD_CALLBACK data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_CHANNELORDER data, std::vector<SoundClass::Sound::ExInfo> &vec);

void AddExInfo(SoundClass::Sound::EXTRA_INFO info, FMOD_SOUNDGROUP* data, std::vector<SoundClass::Sound::ExInfo> &vec);