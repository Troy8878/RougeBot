/*********************************
* fmod.cpp
* Troy
* Created 2014/11/01
* Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
*********************************/

#include <Engine/Common.h>
#include <mruby/class.h>

#include "SoundSystem.h"
#include "SoundManager.h"

// ----------------------------------------------------------------------------

static std::list<ManagedSound> allSounds;
double gvolume = 1.0;
bool gmmute = false;
bool gsmute = false;

struct SoundRef
{
  explicit SoundRef(const ManagedSound &sound)
    : sound(sound)
  {
    allSounds.push_front(sound);
    iter = allSounds.begin();
    
    this->sound->MuteMusic(gmmute);
    this->sound->MuteSound(gsmute);
    this->sound->UpdateGVolume(gvolume);
  }

  NO_ASSIGNMENT_OPERATOR(SoundRef);
  NO_COPY_CONSTRUCTOR(SoundRef);

  ManagedSound sound;

  ~SoundRef()
  {
    allSounds.erase(iter);
  }

  static void UpdateGV(ManagedSound s, double vol)
  {
    s->UpdateGVolume(vol);
  }

  static void MuteSound(ManagedSound s, bool muted)
  {
    s->MuteSound(muted);
    s->UpdateGVolume(gvolume);
  }

  static void MuteMusic(ManagedSound s, bool muted)
  {
    s->MuteMusic(muted);
    s->UpdateGVolume(gvolume);
  }

private:
  typedef decltype(allSounds) SC;
  SC::iterator iter;
};

// ----------------------------------------------------------------------------

mrb_data_type mrb_sound_dt;

static mrb_value mrb_sound_alloc(mrb_state *mrb, ManagedSound sound, RClass *klass);
static mrb_value mrb_sound_new(mrb_state *mrb, mrb_value klass);

static mrb_value mrb_sound_play(mrb_state *mrb, mrb_value self);
static mrb_value mrb_sound_stop(mrb_state *mrb, mrb_value self);
static mrb_value mrb_sound_pause(mrb_state *mrb, mrb_value self);

static mrb_value mrb_sound_playing_p(mrb_state *mrb, mrb_value self);
static mrb_value mrb_sound_paused_p(mrb_state *mrb, mrb_value self);

static mrb_value mrb_sound_volume_get(mrb_state *mrb, mrb_value self);
static mrb_value mrb_sound_volume_set(mrb_state *mrb, mrb_value self);

static mrb_value mrb_sound_music_mute_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_sound_sound_mute_set(mrb_state *mrb, mrb_value self);

void SetMrbGVolume(double vol);
void SetMrbMusicMute(bool muted);
void SetMrbSoundMute(bool muted);

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_sound_init(mrb_state *mrb)
{
  ruby::data_type_init<SoundRef>(mrb_sound_dt, ruby::data_scalar_delete<SoundRef>);

  auto klass = mrb_define_class(mrb, "Sound", mrb->object_class);

  mrb_define_class_method(mrb, klass, "new", mrb_sound_new, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, klass, "load", mrb_sound_new, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, klass, "play", mrb_sound_play, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "stop", mrb_sound_stop, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "pause", mrb_sound_pause, MRB_ARGS_NONE());

  mrb_define_method(mrb, klass, "playing?", mrb_sound_playing_p, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "paused?", mrb_sound_paused_p, MRB_ARGS_NONE());

  mrb_define_method(mrb, klass, "volume", mrb_sound_volume_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "volume=", mrb_sound_volume_set, MRB_ARGS_REQ(1));

  mrb_define_class_method(mrb, klass, "music_muted=", mrb_sound_music_mute_set, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, klass, "sound_muted=", mrb_sound_sound_mute_set, MRB_ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_alloc(mrb_state *mrb, ManagedSound sound, RClass *klass)
{
  auto data = mrb_data_object_alloc(mrb, klass, new SoundRef(sound), &mrb_sound_dt);
  auto obj = mrb_obj_value(data);
  return obj;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_new(mrb_state *mrb, mrb_value klass)
{
  mrb_value data;
  mrb_get_args(mrb, "o", &data);

  auto audioDef = mrb_inst->value_to_json(data);
  auto audio = SoundManager::Instance.Load(audioDef);

  return mrb_sound_alloc(mrb, audio, mrb_class_ptr(klass));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_play(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  sound->Play();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_stop(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  sound->Stop();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_pause(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  sound->Pause();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_playing_p(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  try
  {
    return mrb_bool_value(sound->IsPlaying());
  }
  catch (...)
  {
    return mrb_bool_value(false);
  }
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_paused_p(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  try
  {
    return mrb_bool_value(sound->IsPaused());
  }
  catch (...)
  {
    return mrb_bool_value(false);
  }
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_volume_get(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  return mrb_float_value(mrb, sound->GetVolume());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_volume_set(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  mrb_float volume;
  mrb_get_args(mrb, "f", &volume);

  sound->SetVolume(volume);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_music_mute_set(mrb_state *mrb, mrb_value)
{
  mrb_bool muted;
  mrb_get_args(mrb, "b", &muted);

  SetMrbMusicMute(!!muted);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_sound_mute_set(mrb_state *mrb, mrb_value)
{
  mrb_bool muted;
  mrb_get_args(mrb, "b", &muted);

  SetMrbSoundMute(!!muted);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

void SetMrbGVolume(double vol)
{
  gvolume = vol;
  for (auto &sound : allSounds)
  {
    SoundRef::UpdateGV(sound, gvolume);
  }
}

// ----------------------------------------------------------------------------

void SetMrbMusicMute(bool muted)
{
  gmmute = muted;
  for (auto &sound : allSounds)
  {
    SoundRef::MuteMusic(sound, muted);
  }
}

// ----------------------------------------------------------------------------

void SetMrbSoundMute(bool muted)
{
  gsmute = muted;
  for (auto &sound : allSounds)
  {
    SoundRef::MuteSound(sound, muted);
  }
}

// ----------------------------------------------------------------------------

