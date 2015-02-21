#pragma once

// Frameworks
#import <AVFoundation/AVFoundation.h>
#include <set>
#include <map>
#include <vector>

// Local
#include "AudioManager.h"
#include "OpenALSupport.h"
#include "Animation.h"


// Stands for the iOS
#define MAX_NUMBER_OF_ALSOURCES 32


namespace Furiosity
{
    // Fwd
    class OpenALAudio;
    
    /// The internal state of session
    enum class SessionState
    {
        Uninitialized,
        Playing,
        Saved
    };
}

///
/// A wraper for accesing the AVAudioSession API
///
@interface FRSessionManager : NSObject <AVAudioSessionDelegate>
{
    
}

- (id) initWithAudioManager:(Furiosity::OpenALAudio*) audioManager;

- (void) interrupted:(NSNotification*)notification;

- (void) saveState;

- (void) resumeState;

- (Furiosity::SessionState) sessionState;

@end


namespace Furiosity
{
    class Sound;
    
    ///
    /// SourceInfo, is a simple structure kept per playable channel.
    ///
    struct SourceInfo
    {
        /// The OpenAL source atthached to this info.
        ALuint              Source;
        
        /// Currently playing sound. Null if none
        Sound*              CurrentSound;
        
        SourceInfo() : Source(0), CurrentSound(0) {}
        
        /// Reset this
        void Reset() { CurrentSound = 0; }
    };
    
    
    ///
    /// OpenALAudio is an OpenAL implementation of the AudioManager.
    ///
    class OpenALAudio : public AudioManager
    {
        // Nitty-gritty OpenAL implementation details:
    private:
        vector<SourceInfo>      sources;
        
        ALCcontext*             context;
        ALCdevice*              device;
        UInt32					iPodPlaying;
        bool                    isPlaying;
        
        bool                    enableMusic;
        
        bool                    enableSound;
        
        float                   musicFade           = 0.0f;
        
        FRSessionManager*       sessionManager      = nil;
        
        //AudioPlayer*            musicPlayer         = nil;
        
        // Background music
        NSURL*					musicNextURL;
        string                  bgCurrent;
        AVAudioPlayer*			musicPlayer         = nil;

        
    private:    
        void InitSession();
        
        void InitOpenAL();
        
        SourceInfo* GetNextFree();
        
        bool OpenALInitialized() { return sessionManager.sessionState == SessionState::Playing; }
        
        virtual void PlayMusicImmediately(const string& song, bool loop) override;
        
    // Public interface inherited from AudioManager.
    public:
        OpenALAudio();
        ~OpenALAudio() {}
        
        virtual void Initialize() override;
        virtual void Shutdown() override;
        virtual void Update(float dt) override;
        
        virtual void SetSoundEnabled(bool b) override  { enableSound = b; }
        
        virtual bool SoundEnabled() const override { return enableSound; }
        
        virtual void PlaySound(Sound* sound) override;
        
        virtual void StopSound(Sound* sound) override;
        
        virtual void PauseSound(Sound* sound) override;
        
        virtual void ResumeSound(Sound* sound) override;
        
        virtual void SeekSound(Sound* sound, float time) override;
        
        virtual float GetSoundTime(Sound* sound) override;
        
        virtual SoundState GetSoundState(Sound* sound) override;
        
        ////////////////////////////////////////////////////////////////////////////////
        // Music
        ////////////////////////////////////////////////////////////////////////////////

        virtual void    SetMusicEnabled(bool b) override        { enableMusic = b; }
        
        virtual bool    MusicEnabled() const override           { return enableMusic; }
        
        virtual bool    IsMusicPlaying() const override         { return musicPlayer.isPlaying;     }
        
        virtual void    SetMusicVolume(float vol) override      { [musicPlayer setVolume:vol];      }
        
//        virtual void    PlayMusic(const string& song,
//                                  float fade = 0.0f,
//                                  bool loop = true) override;
        
        // Music stuff
        // virtual float   QueueMusic(const string& song, bool loop = true) override;
        // virtual void    PlayMusic() override;
        // virtual void    ClearQueue() override               { [musicPlayer clearQueue]; }
        
        // virtual void SetFadeTime(float f) override          { musicFade = f;    }
        // virtual float FadeTime() const override             { return musicFade; }
        ///virtual void SetMusicFade(float fromValue, float toValue, float time) override;
        
        // These pauses all sounds, used on Android when the application is minimized.
        virtual void Pause() override;

        // These resumes all sounds, used on Android when the application is maximized.
        virtual void Resume() override;
    
        ALCcontext* Context() { return context; };
    };
}
