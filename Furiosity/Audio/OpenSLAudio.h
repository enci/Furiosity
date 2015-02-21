#pragma once

#ifdef ANDROID

#include <cassert>
#include <string>
#include <set>
#include <jni.h>
#include <algorithm>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <unistd.h>

#include "AudioManager.h"
#include "logging.h"
#include "SoundResource.h"

/*
 * Convenient links where most of the boiler plate code is adapted from:
 *
 * http://www.khronos.org/registry/sles/specs/OpenSL_ES_Specification_1.0.1.pdf
 * http://comments.gmane.org/gmane.comp.handhelds.android.ndk/15310
 * http://ebooks.narotama.ac.id/files/Android%20NDK%20Beginner's%20Guide/Chapter%207%20%20%20Playing%20Sound%20with%20OpenSL%20ES.pdf
 * http://stackoverflow.com/questions/11936702/reading-wav-files-not-reading-header
 *
 */

namespace Furiosity {
    class OpenSLAudio;

    // Wrapped in a namespace, one need not be aware of this.
    namespace Internal {
        class MimePlayer
        {
            SLObjectItf bqPlayerObject;
            SLPlayItf bqPlayerPlay;
            SLVolumeItf bqPlayerVolume;
            SLSeekItf bgSeek;

        public:
            MimePlayer(const std::string& filename, OpenSLAudio* _audioManager, SLEngineItf engineEngine, SLObjectItf outputMixObject);
            void SetGain(const float& gain);
            void Pause();
            void Resume();
            virtual ~MimePlayer();
        };

        class PcmPlayer
        {
        	bool _isPlaying;
			OpenSLAudio* audioManager;
			SLObjectItf bqPlayerObject;
			SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
			SLPlayItf bqPlayerPlay;
			SLVolumeItf bqPlayerVolume;
			SLSeekItf bgSeek;

			float _timeElapsed;

        public:
            PcmPlayer(OpenSLAudio* _audioManager, SLEngineItf engineEngine, SLObjectItf outputMixObject);
            virtual ~PcmPlayer();

            void Seek(const float seconds);
            void Play(Sound* sound);
            void FinishedPlaying();
            bool IsPlaying() const;
            void Stop();
            void SetGain(const float& gain);

            SLuint32 GetState();
            void Pause();
            void Resume();
            float GetProgressTime();

            Sound* sound{0};
        };

        static int PcmByteOffset(float seconds) {
        	// Compute byte offset, the figures are hard coded and fine tuned for
        	// the format we always use.
        	const int samplesPerSecond = 44100;
        	const int bitsPerSample    = 16;
        	const int bytesPerSample   = bitsPerSample / 8;

        	int offset = (seconds * samplesPerSecond * bytesPerSample);

        	// PCM bytes come in bundles of two, the following rounds the number
        	// to the nearest even, assuring the offset aligns with the PCM
        	// byte bundles.
        	offset = (offset | 1) ^ 1;

        	return offset;
        }
    }

    class OpenSLAudio : public AudioManager
    {
        SLObjectItf             engineObject;
        SLEngineItf             engineEngine;
        SLObjectItf             outputMixObject;
        float                   fadeTime;
        float                   fade;
        float                   bgVol;
        bool                    isSoundEnabled; // PCM buffers
        bool                    isMusicEnabled; // MP3
        bool					isSystemMute;   // Android mute, no noise during lock screen.
        std::string             musicFilename;
        std::string             nextMusicFileName;

        Internal::MimePlayer*   music;
        std::set<Internal::PcmPlayer*> players;

        Internal::PcmPlayer* GetNextAvailablePlayer();

        /// Players paused due to the app being hidden.
        std::vector<Internal::PcmPlayer*> paused;
        
        virtual void PlayMusicImmediately(const string& song, bool loop) override;

    public:
        OpenSLAudio();
        void FinishedPlaying(Internal::PcmPlayer* player);

        static void FinishedPlayingCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context) {
        	/// Note: this might be called from a separate thread?
            Internal::PcmPlayer* player = reinterpret_cast<Internal::PcmPlayer*>(context);
            player->FinishedPlaying();
        }

        // Public interface inherited from AudioManager.
        virtual void Initialize();
        virtual void Shutdown();
        virtual void Update(float dt);

//        virtual float QueueMusic(const string& song, bool loop = 1) override;
//        virtual void SetMusicFade(float fromValue, float toValue, float time) override;
//        virtual void ClearQueue() override {};
        virtual void SetMusicEnabled(bool enableMusic);
        virtual bool MusicEnabled() const;
        virtual void SetFadeTime(float f);
        virtual float FadeTime() const;
        virtual bool IsMusicPlaying() const;
        virtual void SetSoundEnabled(bool enableSound);
        virtual bool SoundEnabled() const;
        virtual void SetMusicVolume(float vol);
        virtual void Pause();
        virtual void Resume();
//        virtual bool MusicTrackedFinished() const { return false; }		// TODO: Implement this one
//        virtual void PlayMusic() {};									// TODO: Implement this one

        virtual void PlaySound(Sound* sound) override;
        virtual void StopSound(Sound* sound) override;
        virtual void PauseSound(Sound* sound) override;
        virtual void ResumeSound(Sound* sound) override;
        virtual void SeekSound(Sound* sound, float time) override;
        virtual float GetSoundTime(Sound* sound) override;
        virtual SoundState GetSoundState(Sound* sound) override;
    };
}

#endif
