////////////////////////////////////////////////////////////////////////////////
//  SoundResource.h
//  Furiosity
//
//  Created by Bojan Endrovski on 05/10/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <queue>

#include "Resource.h"
#include "Defines.h"

#ifdef ANDROID
    // Hack to fix Android errors, and remain iOS compatible.
    typedef int ALenum;
    typedef int ALuint;
    typedef size_t ALsizei;
    typedef float ALfloat;
    typedef bool ALboolean;
#endif

#ifdef IOS
#   include "OpenALSupport.h"
#endif


using std::string;
using std::queue;

namespace Furiosity
{
    class ResourceManager;
    
    // Platform stuff
    class OpenALAudio;	// Apple iOS
    class OpenSLAudio;	// Google Android

    
    ///
    /// Holds the playable PCM data, and a few other helpful bits
    ///
    class SoundResource : public Resource
    {
        /// Resource's lifetime is handled by the resource manager
        friend class ResourceManager;
        friend class AudioManager;
        friend class OpenALAudio;
        friend class OpenSLAudio;
        
    protected:

        /// id of the sound buffer in oal
        ALuint      openalDataBuffer;

        /// Book-keeping for the internal format
        ALenum      openalFormat;

        //        float        gain;

        /// Duration of the sound as loaded
        float        duration;
        
#ifdef IOS
        /// iOS OpenAL sample rate (data type varies from Android)
        ALsizei     sampleRate;
#endif

        /// Container for the raw PCM bytes.
        std::vector<char> pcmBytes;
        
    protected:
        /// Ctor from params
        SoundResource(const std::string& filename);
        
        /// Dtor
        virtual ~SoundResource();

        /// Get the OpenAL data buffer id
        ALuint GetOpenALDataBuffer() const { return openalDataBuffer; }

    public:

		#ifdef ANDROID

        /// Properties pertaining to the PCM signal. Initialized with
        /// highly common values.
        unsigned int sampleRate{44100};
		unsigned int numChannels{1};
		unsigned int bitsPerSample{16};
		unsigned int containerSize{16};
		unsigned int channelMask{1};
		unsigned int endianness{2};

	    /// Set the PCM bytes.
		void setPcm(const std::vector<char>& pcm) {
			size      = pcm.size();
			pcmBytes  = pcm;
		}

		#else

		#endif


        /// No pain no gain
        // float Gain() const { return gain; }
        //void SetGain(float g) { gain = g; }
        
        /// Retrieve a reference to the raw PCM bytes
        const std::vector<char>& GetPCM() { return pcmBytes; }

        /// Duration on this sound buffer in seconds
        float Duration() const { return duration; }
    };


    ///
    /// The state in which a certain sound can be in
    ///
    enum class SoundState
    {
        Invalid,
        Stopped,
        Initialized,
        Playng,
        Paused
    };
    
    ///
    /// A base class for the sound and sound queue.
    ///
    class Sound
    {
        friend class AudioManager;
        friend class OpenALAudio;
        friend class OpenSLAudio;
        
    protected:
        /// A unique handle that ties this sound instance to an OpenSL
        /// or OpenAL player.
        ALuint              activeSource    = 0;

        /// Gain or volume
        float               gain            = 1.0f;

        ///
        ALboolean           loop            = false;
        
        /// The sound data to be played
        SoundResource*      soundBuffer     = nullptr;

        /// The audio manager might needs to reset this sound when done playing
        /// Other important stuff for
        virtual void Reset()                { activeSource = 0; };
        
    public:

        /// Create a new sound from a file
        Sound(const string& filename);

        /// Creates a new sound from a loaded sound resource
        Sound(SoundResource* sndResource);

        /// Delete this sound
        virtual ~Sound();

        /// Play the sound
        void Play();

        /// Stop the sound
        void Stop();

        /// Pause a playing sound
        void Pause();
        
        void Resume();

        /// Seek to a position on time given in seconds
        void Seek(const float time);

        /// Get the current time of playing for this sound
        float CurrentTime();

        /// Get the current state of the sound. Check the enumeration for values
        SoundState GetState();

        /// Get the sound resource (data) for this sound
        SoundResource* SoundBuffer() const      { return soundBuffer; }

        /// Get the currently playing source for this sound. Zero if not playing
        ALuint      Source() const              { return activeSource; }

        /// Get the current gain (volume) of this sound
        float       Gain() const                { return gain; }

        /// Set gain (volume) for this sound
        void        SetGain(float g)            { gain = g; }

        /// Check is this sound is set to loop
        ALboolean   Loop() const                { return loop; }

        /// Set this sound to loop or not
        void        SetLoop(ALboolean l)        { loop = l; }


		#ifdef ANDROID
			float seekOffset;
		#endif
    };
}

