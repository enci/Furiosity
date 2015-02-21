////////////////////////////////////////////////////////////////////////////////
//  AudioManager.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/4/11.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "ResourceManager.h"
#include "SoundResource.h"
#include "Animation.h"

using std::string;

namespace Furiosity
{
	// This is a unified public interface for OpenAL and OpenSL.
    class AudioManager : protected Animatable
    {
        
    public:
        
        AudioManager();
        
        virtual ~AudioManager();
        
        /// To act as a constructor
    	virtual void Initialize() = 0;

		/// Actual destructor
    	virtual void Shutdown() = 0;

		/// As all other managers this one needs dt update some internals
    	virtual void Update(float dt) = 0;
        
        ////////////////////////////////////////////////////////////////////////////////
        // Sound manipulation
        ////////////////////////////////////////////////////////////////////////////////
        
        virtual void PlaySound(Sound* sound) = 0;
        
        virtual void StopSound(Sound* sound) = 0;
        
        virtual void PauseSound(Sound* sound) = 0;
        
        virtual void ResumeSound(Sound* sound) = 0;
        
        virtual void SeekSound(Sound* sound, float time) = 0;
        
        virtual float GetSoundTime(Sound* sound) = 0;
        
        virtual SoundState GetSoundState(Sound* sound) = 0;
        
        virtual void SetSoundEnabled(bool b) = 0;
        
		virtual bool SoundEnabled() const = 0;
        
        ////////////////////////////////////////////////////////////////////////////////
        // Music stuff
        ////////////////////////////////////////////////////////////////////////////////
        
    	virtual void SetMusicEnabled(bool b) = 0;
        
    	virtual bool MusicEnabled() const = 0;
        
    	virtual bool IsMusicPlaying() const = 0;
        
        virtual void SetMusicVolume(float vol) = 0;
        
        virtual void PlayMusicImmediately(const string& song, bool loop) = 0;
        
        virtual void PlayMusic(const string& song,
                               float fade = 0.0f,
                               bool loop = true);
        
        ////////////////////////////////////////////////////////////////////////////////
        /// Handlers for system events such as minimize
        ////////////////////////////////////////////////////////////////////////////////
        
        virtual void Pause() = 0;
        
        virtual void Resume() = 0;
    };

    // NB: this is declaration only.
    extern AudioManager& gAudioManager;
}

#endif
