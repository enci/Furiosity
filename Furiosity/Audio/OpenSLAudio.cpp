#include "OpenSLAudio.h"

#ifdef ANDROID

using namespace Furiosity;
using namespace Furiosity::Internal;

/// Calculate the gain based on attenuation.
inline SLmillibel GainToAttenuation(const float gain) {
    return ((gain < 0.01f) ? -96.0F : 20 * log10(gain)) * 100;
}

OpenSLAudio::OpenSLAudio() :
    isSoundEnabled(true),
    isMusicEnabled(true),
    isSystemMute(false),
    music(nullptr),
    bgVol(1)
{}

void OpenSLAudio::Initialize() {
    SLresult result;

    result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    ASSERT(SL_RESULT_SUCCESS == result);

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE); // was set to false? async?
    ASSERT(SL_RESULT_SUCCESS == result);

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &(engineEngine));
    ASSERT(SL_RESULT_SUCCESS == result);

    // With this way can play music, apparently.
    const SLInterfaceID ids[] = {}; // SL_IID_VOLUME <- not in Android's OpenSL version.
    const SLboolean req[] = {};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, ids, req);
    ASSERT(SL_RESULT_SUCCESS == result);

    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    ASSERT(SL_RESULT_SUCCESS == result);


}


void OpenSLAudio::FinishedPlaying(PcmPlayer* player) {
    // Enable the code below to always create a new player. Great for
    // testing if there are memory leaks.
    //players.erase(player);
    //delete player;
}

Internal::PcmPlayer* OpenSLAudio::GetNextAvailablePlayer() {
    for(PcmPlayer* player : players) {
        if(!player->IsPlaying()) {
            return player;
        }
    }

    // The limit is 30, but there might be a MP3 player running.
    if(players.size() < 28) {
        PcmPlayer* player = new PcmPlayer(this, engineEngine, outputMixObject);
        players.insert(player);
        return player;
    }

#ifdef DEBUG
    LOG("OpenSLAudio::getNextAvailablePlayer() Cannot create SoundPlayer, hardcoded limit of 30 players reached.");
#endif

    return 0;
}



/// Actual destructor
void OpenSLAudio::Shutdown() {
    for(PcmPlayer* player : players) {
        SafeDelete(player);
    }

    (*outputMixObject)->Destroy(outputMixObject);
    (*engineObject)->Destroy(engineObject);
}

/// As all other managers this one needs to update some internals
void OpenSLAudio::Update(float dt) {
    if(isMusicEnabled && ! isSystemMute) {

    	for(PcmPlayer* player : players) {
    		if(player->sound) {
    			player->SetGain(player->sound->Gain());
    		}
    	}

        // New song is enqueued:
        if(!nextMusicFileName.empty()) {
            fade -= dt / fadeTime;

            // Create new player when the old player has faded out, or; when
            // there is no player to begin with.
            if(fade <= 0 || music == nullptr) {
                SafeDelete(music);

                music = new MimePlayer(nextMusicFileName, this, engineEngine, outputMixObject);
                musicFilename     = nextMusicFileName;
                nextMusicFileName = "";
                fade              = 0;
            }

            music->SetGain(fade * bgVol);

        // Assuming there is a player, fade in, if it wasn't already.
        } else if(fade < 1 && music != nullptr) {
            fade = std::min<float>(1, fade += dt / fadeTime);
            music->SetGain(fade * bgVol);
        }
    }
}


void OpenSLAudio::PlayMusicImmediately(const string& song, bool loop)
{
    nextMusicFileName = song;
    fade = 0.0f;
}

/*
float OpenSLAudio::QueueMusic(const std::string& song, bool loop) {
    nextMusicFileName = song;
    return 0.0f;
}
*/

//void OpenSLAudio::SetMusicFade(float fromValue, float toValue, float time) {}

void OpenSLAudio::SetMusicEnabled(bool musicEnabled) {

    // No state change:
    if(musicEnabled == isMusicEnabled) {
        return;
    }

    isMusicEnabled = musicEnabled;

    if(musicEnabled) {
        if(!musicFilename.empty()) {
            ; //QueueMusic(musicFilename.c_str());
        }
    } else {
        SafeDelete(music);
    }
}


bool OpenSLAudio::MusicEnabled() const       { return isMusicEnabled; }

void OpenSLAudio::SetFadeTime(float f)       { fadeTime = f; }
float OpenSLAudio::FadeTime() const          { return fadeTime; }

bool OpenSLAudio::IsMusicPlaying() const     { return music != nullptr; }

void OpenSLAudio::SetSoundEnabled(bool enableSound)    {

    // Sound is currently enabled, but we are stopping:
    if(!enableSound) {
        // Stop all currently playing sounds:
        for(PcmPlayer* player : players) {
            player->Stop();
        }
    }

    isSoundEnabled = enableSound;
}

bool OpenSLAudio::SoundEnabled() const       { return isSoundEnabled; }
void OpenSLAudio::SetMusicVolume(float vol)  { bgVol = vol; }


void OpenSLAudio::Pause() {
	isSystemMute = true;
    if(music != nullptr) {
        music->Pause();
    }

    // Take note of currently playing sounds
    for(PcmPlayer* player : players) {
    	// Make sure it's playing
    	if(player->IsPlaying()) {
    		// Make sure it's not paused
    		if(SL_PLAYSTATE_PLAYING == player->GetState()) {
    			player->Pause();
    			paused.push_back(player);
    		}
    	}
    }
}

void OpenSLAudio::Resume() {
	isSystemMute = false;
    if(music != nullptr) {
        music->Resume();
    }

    // Resume sounds that were playing
    while( ! paused.empty()) {
    	paused.back()->Resume();
    	paused.pop_back();
    }
}

PcmPlayer::PcmPlayer(OpenSLAudio* _audioManager, SLEngineItf engineEngine, SLObjectItf outputMixObject)
	: audioManager(_audioManager)
	, _isPlaying(false)
    , _timeElapsed(0) {
    SLresult result;


     SLDataFormat_PCM format_pcm = {
                SL_DATAFORMAT_PCM,              // formatType
                //2,
                1,                              // numChannels
                SL_SAMPLINGRATE_44_1,           // samplesPerSec
                SL_PCMSAMPLEFORMAT_FIXED_16,    // bitsPerSample
                16,                             // containerSize
                SL_SPEAKER_FRONT_CENTER,        // channelMask
                //SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                SL_BYTEORDER_LITTLEENDIAN       // endianness
     };

     SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {
               // We're feeding raw PCM bytes, can change mode to file handler
               // here, too. But we already got resource management covered.
               SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
               // Number of buffers, we use "1", I'd rather miss a sound
               // than play it delayed. Though, we have our own "IsPlaying"
               // flag, so this should not be reached.
               1
       };



    SLDataSource audioSrc = { &loc_bufq, &format_pcm };

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, nullptr};

    // create audio player
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};

    const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    // NB: This only works on the arch that the application was build for.
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 3, ids, req);
    ASSERT(SL_RESULT_SUCCESS == result);

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    ASSERT(SL_RESULT_SUCCESS == result);

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    ASSERT(SL_RESULT_SUCCESS == result);

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    ASSERT(SL_RESULT_SUCCESS == result);

    // Register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, &OpenSLAudio::FinishedPlayingCallback, this);
    ASSERT(SL_RESULT_SUCCESS == result);

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    ASSERT(SL_RESULT_SUCCESS == result);

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    ASSERT(SL_RESULT_SUCCESS == result);
}

PcmPlayer::~PcmPlayer() {
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
    (*bqPlayerObject)->Destroy(bqPlayerObject);
    bqPlayerObject      = 0;
    bqPlayerPlay        = 0;
    bqPlayerBufferQueue = 0;
    sound               = 0;
    _isPlaying          = false;
}

void PcmPlayer::FinishedPlaying() {

	// Play again!
	if(sound->Loop()) {
		Play(sound);

		// Play retains a new copy. Let's release one.
		gResourceManager.ReleaseResource(sound->SoundBuffer());

		LOG("Looping sound(%s)", sound->SoundBuffer()->Path().c_str());

	// Stop playing.
	} else {
		_isPlaying = false;
		SLresult result = (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
		ASSERT(SL_RESULT_SUCCESS == result);

		audioManager->FinishedPlaying(this);
		gResourceManager.ReleaseResource(sound->SoundBuffer());
		sound = 0;
	}
}

bool PcmPlayer::IsPlaying() const {
    // It's pointless using the play state here, "SL_PLAYSTATE_PLAYING",
    // simply indicates it's ready to play, not that it's actually playing.
    // instead we use the finished-playing callback to toggle a bool flag
    return _isPlaying;
}

void PcmPlayer::Stop() {

	SoundResource* b = sound->SoundBuffer();

    SLresult result;

	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
	ASSERT(SL_RESULT_SUCCESS == result);

    // This is untested code - Bam fu doesn't use this.
    result = (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
    ASSERT(SL_RESULT_SUCCESS == result);

	gResourceManager.ReleaseResource(sound->SoundBuffer());

    _isPlaying = false;
    sound = 0;
}

void PcmPlayer::Play(Sound* sound)
{

    _isPlaying  = true;
    this->sound = sound;

    // Retain a copy while playing.
    gResourceManager.RetainResource(sound->SoundBuffer());

    const std::vector<char>& pcmBytes = sound->SoundBuffer()->GetPCM();

    int offset = 0;

    // Seek was called before play.
	const float seek = sound->seekOffset;

    if(seek > 0) {
    	offset = Furiosity::Internal::PcmByteOffset(seek);

    	if(offset >= pcmBytes.size()) {
    		offset = pcmBytes.size() - 1;

    		LOG("PcmPlayer::Play(%f) - seek offset out-of-bounds.", seek);
    	}

    	LOG("PcmPlayer::Play(%f) at %d/%d bytes offset", seek, offset, (int)pcmBytes.size());

    	_timeElapsed = sound->seekOffset;

    	// Reset seek offset, next time start at the beginning.
    	sound->seekOffset = 0;
    } else {
    	_timeElapsed = 0;
    }

	SLresult result;
	result = (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
    ASSERT(SL_RESULT_SUCCESS == result);

    // Pass the PCM to OpenSL.
    result  = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, &pcmBytes[0] + offset, pcmBytes.size() - offset);
    ASSERT(SL_RESULT_SUCCESS == result);

    // Set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    ASSERT(SL_RESULT_SUCCESS == result);
}

/// Seek when already playing.
void PcmPlayer::Seek(const float seconds)
{
	int offset = Furiosity::Internal::PcmByteOffset(seconds);

    const std::vector<char>& pcmBytes = sound->SoundBuffer()->GetPCM();

    // Clamp to the end-of-file offset.
	if(offset >= pcmBytes.size()) {
		offset = pcmBytes.size() - 1;

		LOG("PcmPlayer::Seek(%f) - seek offset out-of-bounds.", seconds);
	}

	// Reset the seek offset, just in case.
	sound->seekOffset = 0;

	// Store duration:
	_timeElapsed = seconds;

	SLresult result;
	result = (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
    ASSERT(SL_RESULT_SUCCESS == result);

    // Pass the PCM to OpenSL.
    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, &pcmBytes[0] + offset, pcmBytes.size() - offset);
    ASSERT(SL_RESULT_SUCCESS == result);
}


void PcmPlayer::SetGain(const float& gain)
{
    SLmillibel vol = GainToAttenuation(gain);
    SLresult result = (*bqPlayerVolume)->SetVolumeLevel(bqPlayerVolume, vol);
    ASSERT(SL_RESULT_SUCCESS == result);

    //LOG("SetGain(%f): %s", gain, sound->SoundBuffer()->Path().c_str());
}

SLuint32 PcmPlayer::GetState() {
	SLuint32 state;

	SLresult result = (*bqPlayerPlay)->GetPlayState(bqPlayerPlay, &state);
	ASSERT(SL_RESULT_SUCCESS == result);

	return state;
}

void PcmPlayer::Pause() {
	SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	ASSERT(SL_RESULT_SUCCESS == result);
}

void PcmPlayer::Resume() {
	SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	ASSERT(SL_RESULT_SUCCESS == result);
}

float PcmPlayer::GetProgressTime() {
	// Taken from: OpenSL ES 1.0.1 Specification page 345

	SLmillisecond time;

	SLresult result = (*bqPlayerPlay)->GetPosition(bqPlayerPlay, &time);
	ASSERT(SL_RESULT_SUCCESS == result);

	// Scale to seconds.
	return time * 0.001 + _timeElapsed;
}

MimePlayer::MimePlayer(const std::string& filename, OpenSLAudio* _audioManager, SLEngineItf engineEngine, SLObjectItf outputMixObject) {
	SLresult result;

	SLDataFormat_MIME format_pcm = {
		   SL_DATAFORMAT_MIME,
		   nullptr,
		   SL_CONTAINERTYPE_UNSPECIFIED
	};

	SLDataSource audioSrc;
	audioSrc.pFormat = &format_pcm;

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk = {&loc_outmix, nullptr};

	// Audio player interfaces:
	const SLInterfaceID ids[] = { SL_IID_VOLUME, SL_IID_SEEK };
	const SLboolean req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
	const int interfaceCount = 2;

	// Varying per path different loading routines are required.
	if( ! Furiosity::gResourceManager.IsAndroidApkPath(filename)) {
		SLDataLocator_URI loc_bufq = {
			SL_DATALOCATOR_URI,
			(SLchar*) filename.c_str(),
		};

		audioSrc.pLocator = &loc_bufq;
		result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, interfaceCount, ids, req);
		ASSERT(SL_RESULT_SUCCESS == result);

	} else {
		AAsset* asset = AAssetManager_open(Furiosity::gResourceManager.GetAndroidAssetManager(), filename.c_str(), AASSET_MODE_UNKNOWN);

		if(asset == nullptr) {
			ERROR("MimePlayer(%s) - file not found in the APK, or unable to open.", filename.c_str());
		}

		off_t offset = 0, length = 0;

		int fd = dup(AAsset_openFileDescriptor(asset, &offset, &length));

		SLDataLocator_AndroidFD loc_bufq = {
				SL_DATALOCATOR_ANDROIDFD,
				fd,
				offset,
				length
		};

		audioSrc.pLocator = &loc_bufq;

		result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, interfaceCount, ids, req);
		ASSERT(SL_RESULT_SUCCESS == result);
	}

	// realize the player
	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	ASSERT(SL_RESULT_SUCCESS == result);

	// get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
	ASSERT(SL_RESULT_SUCCESS == result);

	// get the volume interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
	ASSERT(SL_RESULT_SUCCESS == result);

	// Acquire the "seek" interface, used for looping:
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_SEEK, &bgSeek);
	ASSERT(SL_RESULT_SUCCESS == result);

	// Enable repeat:
	result = (*bgSeek)->SetLoop(bgSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
	ASSERT(SL_RESULT_SUCCESS == result);

	// set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	ASSERT(SL_RESULT_SUCCESS == result);
}

MimePlayer::~MimePlayer() {
	(*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	(*bqPlayerObject)->Destroy(bqPlayerObject);
	bqPlayerObject      = 0;
	bqPlayerPlay        = 0;
}

void MimePlayer::SetGain(const float& gain) {
	SLmillibel vol = GainToAttenuation(gain);
	SLresult result = (*bqPlayerVolume)->SetVolumeLevel(bqPlayerVolume, vol);
	ASSERT(SL_RESULT_SUCCESS == result);
}

void MimePlayer::Pause() {
	SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	ASSERT(SL_RESULT_SUCCESS == result);
}

void MimePlayer::Resume() {
	SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	ASSERT(SL_RESULT_SUCCESS == result);
}

void OpenSLAudio::PlaySound(Sound* sound) {
	LOG("OpenSLAudio::PlaySound");

	if(isSoundEnabled && ! isSystemMute) {

		if(sound->activeSource == 0) {

			PcmPlayer* p = GetNextAvailablePlayer();

			// We're limited to 30 sounds, so check if there really is a player.
			if(p != 0) {
				p->Play(sound);
			}
		} else {
			// Sound instance is already playing
		}
	}
}

void OpenSLAudio::StopSound(Sound* sound) {
	LOG("OpenSLAudio::StopSound");

	if(isSoundEnabled) {

		// NB: "activeSource" is an OpenAL thing. OpenSL doesn't use it.
		//if(sound->activeSource != 0) {
			bool hasStopped = false;

			for(PcmPlayer* player : players) {
				if(player->sound == sound) {
					player->Stop();
					hasStopped = true;
				}
			}

			if( ! hasStopped) {
				LOG("OpenSLAudio::StopSound tried to stop a sound that was never played?");
			}

		//} else {
			// Sound instance is already stopped
		//}
	}
}

void OpenSLAudio::PauseSound(Sound* sound) {
	LOG("OpenSLAudio::PauseSound");
	for(PcmPlayer* player : players) {
		if(player->sound == sound) {
			player->Pause();
		}
	}
}

void OpenSLAudio::ResumeSound(Sound* sound) {
	LOG("OpenSLAudio::ResumeSound");
	for(PcmPlayer* player : players) {
		if(player->sound == sound) {
			player->Resume();
		}
	}
}

void OpenSLAudio::SeekSound(Sound* sound, float time) {
	LOG("OpenSLAudio::SeekSound(%f)", time);

	bool found = false;

	for(PcmPlayer* player : players) {
		if(player->sound == sound) {
			player->Seek(time);
			found = true;
		}
	}

	if( ! found) {
		// Schedule a seek offset for the next Play() call.
		sound->seekOffset = time;
	}
}

float OpenSLAudio::GetSoundTime(Sound* sound) {
    //LOG("OpenSLAudio::GetSoundTime()");
	float time = 0.0f;

	for(PcmPlayer* player : players) {
		if(player->sound == sound) {
			time = player->GetProgressTime();
		}
	}

    return time;
}

SoundState OpenSLAudio::GetSoundState(Sound* sound) {
	LOG("OpenSLAudio::GetSoundState()");

	for(PcmPlayer* player : players) {
		if(player->sound == sound) {

			SLuint32 state = player->GetState();

			if(state == SL_PLAYSTATE_PLAYING) {
				return SoundState::Playng;
			} else if(state == SL_PLAYSTATE_STOPPED) {
				return SoundState::Stopped;
			} else if(state == SL_PLAYSTATE_PAUSED) {
				return SoundState::Paused;
			}

			// OpenSL returned an unknown state, probably will
			// never happen.
			return SoundState::Invalid;
		}
	}

	// No source attached, return invalid state.
	return SoundState::Invalid;
}



#endif // endif ANDROID
