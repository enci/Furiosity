#include "AudioManager.h"

#ifdef ANDROID
#   include "OpenSLAudio.h"

    // Fill the AudiManager global with an OpenSL implementation:
    Furiosity::AudioManager& Furiosity::gAudioManager = *(new Furiosity::OpenSLAudio());

#else
#   include "OpenALAudio.h"
    // Fill the AudiManager global with an OpenAL implementation:
    Furiosity::AudioManager& Furiosity::gAudioManager = *(new Furiosity::OpenALAudio());
#endif

namespace Furiosity
{
    AudioManager::AudioManager()
    {
        
    }
    
    AudioManager::~AudioManager()
    {
        // Just here to spread some virtual polymorphic goodness.
    }
    
    void AudioManager::PlayMusic(const string& song,
                                float fade,
                                bool loop)
    {
        if(fade > 0.0f)
        {
            // string songCpy = song;
            
            // Use animation for this
            auto anim = new AnimationSequence({
                new ParameterAnimation<AudioManager, float>(this,
                                                            &AudioManager::SetMusicVolume,
                                                            1.0f,
                                                            0.0f,
                                                            fade),
                new EventAnimation([this, song, loop]{ this->PlayMusicImmediately(song, loop); }),
                new ParameterAnimation<AudioManager, float>(this,
                                                            &AudioManager::SetMusicVolume,
                                                            0.0f,
                                                            1.0f,
                                                            fade),
            });
            AddAnimation("Play", anim);
            PlayAnimation("Play");
        }
        else
        {
            PlayMusicImmediately(song, loop);
        }
    }
}
