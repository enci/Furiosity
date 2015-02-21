#include "OpenALAudio.h"
#include "SoundResource.h"

using namespace std;
using namespace Furiosity;


////////////////////////////////////////////////////////////////////////////////
//                                  Macros
////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

string ALErrorString(GLenum error)
{
    switch( error )
    {
		case AL_NO_ERROR:
            return "AL_NO_ERROR There is not currently an error";
            
		case AL_INVALID_NAME:
			return "AL_INVALID_NAME A bad name (ID) was passed to an OpenAL function";
            
		case AL_INVALID_ENUM:
			return "GL_INVALID_OPERATION An invalid enum value was passed to an OpenAL function";
            
		case AL_INVALID_VALUE:
            return "AL_INVALID_VALUE An invalid value was passed to an OpenAL function";
            
		case AL_INVALID_OPERATION:
            return "AL_INVALID_OPERATION The requested operation is not valid";
            
        case AL_OUT_OF_MEMORY:
            return "AL_OUT_OF_MEMORY the requested operation resulted in OpenAL running out of memory";
            
		default:
            return "UNKNOWN";
    }
    
    return "UNKNOWN";
}

#define AL_GET_ERROR(msg)                                           \
{                                                                   \
    ALenum error;                                                   \
    bool err = false;                                               \
    while ( (error = alGetError()) != AL_NO_ERROR)                  \
    {                                                               \
        LOG( "OpenAL ERROR: %s\n %s CHECK POINT: %s (line %d)\n",   \
        ALErrorString(error).c_str(), (msg),                        \
        __FILE__, __LINE__ );                                       \
        err = true;                                                 \
    }                                                               \
}

#else
#define AL_GET_ERROR(msg)
#endif

@implementation FRSessionManager
{
    Furiosity::OpenALAudio* _audioManager;
    
    SessionState            _state;
}

-(id) initWithAudioManager:(Furiosity::OpenALAudio*) audioManager
{
    self    = [super init];
    _state  = SessionState::Uninitialized;
    
    if(self)
    {
        _audioManager = audioManager;
        
        AVAudioSession* instance = [AVAudioSession sharedInstance];
                
        NSError* err    = nil;
        bool success    = NO;
        
        // This is the most suitable category for game I would imagine
        success = [instance setCategory:AVAudioSessionCategorySoloAmbient error:&err];
        ASSERT(success);
        
        success = [instance setActive:YES error:&err];
        ASSERT(success);
        
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(interrupted:) name:AVAudioSessionInterruptionNotification object:nil];
        
        _state  = SessionState::Playing;
    }
    
    return self;
}

-(void)interrupted:(NSNotification*) notification
{
    long unsigned int type = [[[notification userInfo]
                               objectForKey:AVAudioSessionInterruptionTypeKey] unsignedIntegerValue];

#ifdef DEBUG
    NSDictionary*   info = [notification userInfo];
    
    NSString*       name = [notification name];
    
    id              obj  = [notification object];
    
    NSLOG(@"info: %@", info);
    NSLOG(@"name: %@", name);
    NSLOG(@"obj: %@", obj);
#endif
    
    
    AVAudioSessionInterruptionType interruptionType = (AVAudioSessionInterruptionType)type;
    
    if(interruptionType == AVAudioSessionInterruptionTypeBegan)
    {
        LOG("AVAudioSessionInterruptionTypeBegan")
        [self saveState];
    }
    else if(interruptionType == AVAudioSessionInterruptionTypeEnded)
    {
        LOG("AVAudioSessionInterruptionTypeEnded")
        [self resumeState];
    }
}

- (void) beginInterruption
{
    LOG("beginInterruption");
}

- (void) endInterruption
{
    LOG("endInterruption");
}

-(void) saveState
{
    if(_state  == SessionState::Playing)
    {
        NSError* err    = nil;
        bool success    = NO;
        
        success = [[AVAudioSession sharedInstance] setActive:NO error:&err];
        
        if (!success)
        {
            NSLOG(@"%@", [err localizedDescription]);
            return;
        }
        
        
        success = alcMakeContextCurrent(NULL);
        if (!success)
        {
            NSLOG(@"%@", [err localizedDescription]);
            return;
        }
        
        alcSuspendContext(_audioManager->Context());
        
        LOG("Saved playing AVAudioSession.");
        
        _state = SessionState::Saved;
    }
    else
        LOG("Trying to interupt a AVAudioSession from non-playing state!");
}


-(void) resumeState
{
    if(_state  == SessionState::Saved)
    {
        NSError* err    = nil;
        bool success    = NO;

        success = [[AVAudioSession sharedInstance] setActive:YES error:&err];
        if (!success)
        {
            NSLOG(@"%@", [err localizedDescription]);
            return;
        }
        
        success = alcMakeContextCurrent(_audioManager->Context());
        if (!success)
        {
            NSLOG(@"%@", [err localizedDescription]);
            return;
        }
        
        alcProcessContext(_audioManager->Context());
        
        LOG("Resumed saved AVAudioSession.");
        _state = SessionState::Playing;
    }
    else
        LOG("Trying to resume AVAudioSession from non-saved state!");
}

- (Furiosity::SessionState) sessionState
{
    return _state;
}


@end


////////////////////////////////////////////////////////////////////////////////
// Listeners and events
////////////////////////////////////////////////////////////////////////////////

void interruptionListener(void *	inClientData,
                          UInt32	inInterruptionState)
{
    OpenALAudio* audio = (OpenALAudio*) & Furiosity::gAudioManager;

    LOG("If the application doesn't crash - it works! probably.");

    if (inInterruptionState == kAudioSessionBeginInterruption)
    {
        AudioSessionSetActive(NO);
        alcMakeContextCurrent(NULL);
        alcSuspendContext(audio->Context());
    }
    else if (inInterruptionState == kAudioSessionEndInterruption)
    {
        OSStatus result = AudioSessionSetActive(YES);
        
        if(result)
        {
            LOG("Error setting audio session active! %d\n", (int)result);
        }
        
        AudioSessionSetActive(YES);
        alcMakeContextCurrent(audio->Context());
        alcProcessContext(audio->Context());
    }
}

void RouteChangeListener(void *                  inClientData,
                         AudioSessionPropertyID  inID,
                         UInt32                  inDataSize,
                         const void *            inData)
{
    //CFDictionaryRef dict = (CFDictionaryRef)inData;
    
    /*
     CFStringRef oldRoute = (CFDictionaryRef)CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
     
     UInt32 size = sizeof(CFStringRef);
     
     CFStringRef newRoute;
     OSStatus result = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &newRoute);
     
     NSLog(@"result: %d Route changed from %@ to %@", result, oldRoute, newRoute);
     */
}

////////////////////////////////////////////////////////////////////////////////
//                              OpenALAudio
////////////////////////////////////////////////////////////////////////////////
OpenALAudio::OpenALAudio() :
    context(0),
    device(0),
    iPodPlaying(false),
    isPlaying(false),
    enableMusic(true),
    enableSound(true)
{
    
}
    
void OpenALAudio::Initialize()
{
    InitSession();
    InitOpenAL();
}

void OpenALAudio::InitSession()
{
    sessionManager = [[FRSessionManager alloc] initWithAudioManager:this];
    
    /*
    // setup our audio session
    OSStatus result = AudioSessionInitialize(NULL, NULL, interruptionListener, this);
    if(result)
    {
        LOG("Error initializing audio session! %d\n", (uint)result);
    }
    else
    {
        // If there is other audio playing, we don't want to play the background music
        UInt32 size = sizeof(iPodPlaying);
        result = AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &size, &iPodPlaying);
        if (result)
            printf("Error getting other audio playing property! %d", (uint)result);
        
        // If the iPod is playing, use the ambient category to mix with it
        // otherwise, use solo ambient to get the hardware for playing the app background track
        UInt32 category = (iPodPlaying) ? kAudioSessionCategory_AmbientSound : kAudioSessionCategory_SoloAmbientSound;
        
        result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        if (result)
            printf("Error setting audio session category! %d\n", (uint)result);
        
        result = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, RouteChangeListener, this);
        if (result)
            printf("Couldn't add listener: %d", (uint)result);
        
        result = AudioSessionSetActive(true);
        if (result)
            printf("Error setting audio session active! %d\n", (uint)result);
    }
    */
    
    
//    musicPlayer = [[AudioPlayer alloc] init];
}

void OpenALAudio::InitOpenAL()
{
    AL_GET_ERROR("Error stack not empty!");

    sources.resize(MAX_NUMBER_OF_ALSOURCES);
    
    // Create a new OpenAL Device
    // Pass NULL to specify the system’s default output device
    device = alcOpenDevice(NULL);
    AL_GET_ERROR("Error creating device!");
    
    if (device != NULL)
    {
        // Create a new OpenAL Context
        // The new context will render to the OpenAL Device just created
        context = alcCreateContext(device, 0);
        AL_GET_ERROR("Error opening source!");
        
        if (context != NULL)
        {
            // Make the new context the Current OpenAL Context
            alcMakeContextCurrent(context);
            AL_GET_ERROR("Error Setting context!");
            
            // Create all sources on start up
            ALuint allSources[MAX_NUMBER_OF_ALSOURCES];
            // Fill 'em up
            alGenSources(MAX_NUMBER_OF_ALSOURCES, allSources);
            AL_GET_ERROR("Error generating sources!");
            //
            for (int i = 0; i < MAX_NUMBER_OF_ALSOURCES; i++)
                sources[i].Source = allSources[i];
            
            //squeueSource = allSources[0];
        }
        else
        {
            printf("Error opening OpenAL context!\n");
        }
    }
    else
    {
        printf("Error opening OpenAL device!\n");
    }
    
    // clear any errors
    AL_GET_ERROR("Error stack not empty!");
}

/// Actual destructor
void OpenALAudio::Shutdown()
{
    // TODO: Release some resources here
}


SourceInfo* OpenALAudio::GetNextFree()
{
    AL_GET_ERROR("Error stack not empty!");
    
    ALint state;
//    SourceInfo* info = 0;
    
    for(int i = 0; i < MAX_NUMBER_OF_ALSOURCES; ++i)
    {
        SourceInfo* si = &sources[i];
        alGetSourcei(si->Source, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");
        //
        if (state != AL_PLAYING)
        {
            return si;
            //info = &si;
            //return info;
            
        }
    }
    
    return 0;
}


/// As all other managers this one needs dt update some internals
void OpenALAudio::Update(float dt)
{
    Animatable::Update(dt);
    
    if(sessionManager.sessionState != SessionState::Playing)
        [sessionManager resumeState];
    
    if(sessionManager.sessionState != SessionState::Playing)
        return;
    
    AL_GET_ERROR("Error stack not empty!");
    
    ALint state;
    //
    for(int i = 0; i < MAX_NUMBER_OF_ALSOURCES; ++i)
    {
        // Get reference to the source info
        SourceInfo& si = sources[i];
        
        // The the state of the source
        alGetSourcei(si.Source, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");
        
        if (!si.CurrentSound)
            continue;
        
        // Set gain
        alSourcef(si.Source, AL_GAIN, si.CurrentSound->Gain());
        AL_GET_ERROR("Error setting gain!");
        
        // Check if not playing
        if (state != AL_PLAYING &&
            state != AL_PAUSED)
        {
            Sound* sound = static_cast<Sound*>(si.CurrentSound);        // Get the Sound
            gResourceManager.ReleaseResource(sound->SoundBuffer());     // Release the hook to the buffer
            sound->Reset();                                             // Reset the sound
            si.Reset();
            
            //LOG("Releasing expired non playing sound");                                                                                                // Reset the info
        }
    }
    
    if(!MusicEnabled() && musicPlayer != nil)
        [musicPlayer stop];
    
    if(MusicEnabled() && musicPlayer != nil)
        [musicPlayer play];
}


void OpenALAudio::PlaySound(Sound* sound)
{
    if(!OpenALInitialized())
        return;
    
    if(!enableSound)
        return;
    
    //TODO: Check if already playing
    
    SourceInfo* info = GetNextFree();
    if(info == 0)
        return; // Silent (hehehe) skip
    
    LOG("Playing sound: %i, playing at source: %i",
        sound->SoundBuffer()->GetOpenALDataBuffer(),
        info->Source);
    
    // Set the source info to point to this one
    info->CurrentSound = sound;
    
    gResourceManager.RetainResource(sound->SoundBuffer());
    
    ALuint source = info->Source;
    sound->activeSource = source;
    
    alSourcei(source, AL_LOOPING, sound->Loop());
    AL_GET_ERROR("Error setting looping!");
    
    alSourcef(source, AL_GAIN, sound->Gain());
    AL_GET_ERROR("Error setting gain!");
    
    // attach OpenAL Buffer to OpenAL Source
    ASSERT(sound->SoundBuffer());
    alSourcei(source, AL_BUFFER, sound->SoundBuffer()->GetOpenALDataBuffer());
    AL_GET_ERROR("Error setting source!");
    
    
    // Get state
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    AL_GET_ERROR("Error getting source!");
    //ASSERT(state == AL_INITIAL);
    
    // alSourcef(source, AL_SEC_OFFSET, 1.0f);
    // AL_GET_ERROR("Error seeking!");
    
    /*
    if(state != AL_STOPPED)
    {
        alSourceStop(source);
        AL_GET_ERROR("Unable to stop buffer");
    }
    */

    
    // Play the sound
    alSourcePlay(source);
    //
    ALenum error;
    if((error = alGetError()) != AL_NO_ERROR)
        printf("Error starting source: %x\n", error);
    else
        this->isPlaying = YES; // Mark our state as playing
}

void OpenALAudio::StopSound(Sound *sound)
{
    if(!OpenALInitialized())
        return;
    
    LOG("Stopping sound: %i, playing at source: %i",
        sound->SoundBuffer()->GetOpenALDataBuffer(),
        sound->Source());
    
    // Stop the buffer from playing and detach it
    ALuint sourceID = sound->Source();
    
    // Check if the sound is actually playing or paused.
    if(sourceID != 0)
    {
        auto buffer = sound->SoundBuffer();
        ASSERT(buffer);
        
        // Get state
        ALint state;
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");
        
        if(state != AL_STOPPED)
        {
            alSourceStop(sourceID);
            AL_GET_ERROR("Unable to stop buffer");
        }
        
        // Assume still attached
        alSourcei(sourceID, AL_BUFFER, 0);
        AL_GET_ERROR("Unable detach buffer");
        
        // Find the channel and reset it
        for(int i = 0; i < MAX_NUMBER_OF_ALSOURCES; ++i)
        {
            if(sources[i].Source == sound->activeSource)
            {
                sources[i].Reset();
                break;
            }
        }
        
        // Unlink the sound from the source
        sound->Reset();
        
        // Manually release the buffer
        gResourceManager.ReleaseResource(buffer);
    }
}

void OpenALAudio::PauseSound(Sound *sound)
{
    if(!OpenALInitialized())
        return;
    
    LOG("Pausing sound: %i, playing at source: %i",
        sound->SoundBuffer()->GetOpenALDataBuffer(),
        sound->Source());

    // Get the source
    ALuint sourceID = sound->Source();
        
    // Check if sound is actually playing or paused.
    if(sourceID != 0)
    {
        auto buffer = sound->SoundBuffer();
        ASSERT(buffer);
    
        // Get state
        ALint state;
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");

        // Stop the source
        if(state == AL_PLAYING)
        {
            alSourcePause(sourceID);
            AL_GET_ERROR("Unable to pause buffer");
        }
    }
}

void OpenALAudio::ResumeSound(Sound* sound)
{
    if(!OpenALInitialized())
        return;
    
    if(!enableSound)
        return;
    
    LOG("Resuming sound: %i, playing at source: %i",
        sound->SoundBuffer()->GetOpenALDataBuffer(),
        sound->Source());
    
    // Get the source
    ALuint sourceID = sound->Source();
    
    // Check if sound is actually playing or paused.
    if(sourceID != 0)
    {
        auto buffer = sound->SoundBuffer();
        ASSERT(buffer);

        // Get state
        ALint state;
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");
        
        // Stop the source
        if(state != AL_PLAYING)
        {
            alSourcePlay(sourceID);
            AL_GET_ERROR("Unable to resume buffer");
        }
    }
}


void OpenALAudio::SeekSound(Sound *sound, float time)
{
    if(!OpenALInitialized())
        return;
    
    LOG("Seeking in sound: %i, playing at source: %i",
        sound->SoundBuffer()->GetOpenALDataBuffer(),
        sound->Source());
    
    // Stop the buffer from playing and detach it
    ALuint sourceID = sound->Source();
    
    // Check if the sound is actually playing or paused.
    if(sourceID != 0)
    {
        alSourcef(sourceID, AL_SEC_OFFSET, time);
        AL_GET_ERROR("Unable to seek sound source!");
        
        /*
        
        auto buffer = sound->SoundBuffer();
        ASSERT(buffer);
        
        // Get state
        ALint state;
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");
        
        if(state != AL_STOPPED)
        {
            alSourceStop(sourceID);
            AL_GET_ERROR("Unable to stop buffer");
        }
        
        // Assume still attached
        alSourcei(sourceID, AL_BUFFER, 0);
        AL_GET_ERROR("Unable detach buffer");
        
        /// Get the state again
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
        AL_GET_ERROR("Error getting source!");
        
        //alSourceRewind(<#ALuint sid#>)
        //AL_GET_ERROR("Error getting source!");
        
        // TODO: Unlink the sound from the source
        //        sound->activeSource = 0;
        
        // Manually release the buffer
        gResourceManager.ReleaseResource(buffer);
        */
    }
}


SoundState OpenALAudio::GetSoundState(Sound* sound)
{
    if(!OpenALInitialized())
        return SoundState::Invalid;
    
    LOG("Getting state for sound: %i, playing at source: %i",
        sound->SoundBuffer()->GetOpenALDataBuffer(),
        sound->Source());
    
    // Get the source
    ALuint sourceID = sound->Source();
    
    // Check if sound is actually playing or paused.
    if(sourceID == 0)
        return SoundState::Invalid;
    
    // Get state
    ALint state;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    AL_GET_ERROR("Error getting source!");
    
    // Stop the source
    switch (state)
    {
        case AL_INITIAL:
            return SoundState::Initialized;
            
        case AL_STOPPED:
            return SoundState::Stopped;
            
        case AL_PAUSED:
            return SoundState::Paused;

        case AL_PLAYING:
            return SoundState::Playng;
    }
    
    return SoundState::Invalid;
}

float OpenALAudio::GetSoundTime(Sound *sound)
{
    // TODO: Implement this!
    
    if(!OpenALInitialized())
        return 0.0f;
    
    float time = -1.0f;
    
    // Stop the buffer from playing and detach it
    ALuint sourceID = sound->Source();
    
    // Check if the sound is actually playing or paused.
    if(sourceID != 0)
    {
        alGetSourcef(sourceID, AL_SEC_OFFSET, &time);
        AL_GET_ERROR("Unable to retrieve seek from sound source!");
    }
    
    return time;
}

void OpenALAudio::PlayMusicImmediately(const string& song, bool loop)
{
    string fullpath = gResourceManager.GetPath(song);
    NSString* path = [NSString stringWithUTF8String:fullpath.c_str()];
    NSURL* pathURL = [[NSURL alloc] initFileURLWithPath: path];
    //
    if(musicPlayer != nil)
    {
        [musicPlayer stop];
        [musicPlayer release];
    }
    musicPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:pathURL error:nil];
    [musicPlayer play];
    if(loop)
        musicPlayer.numberOfLoops = -1;
}

/*
float OpenALAudio::QueueMusic(const string& song, bool loop)
{
    NSString* path = [NSString stringWithUTF8String:song.c_str()];
    bgNextURL = [[NSURL alloc] initFileURLWithPath: path];
    //
    bgCurrent = song;

    NSString* path      = [NSString stringWithUTF8String:song.c_str()];
    AudioSound* sound   = [musicPlayer addSoundFromFile:path loop:-1];
    ASSERT(sound);
    return sound.duration;
}

void OpenALAudio::PlayMusic()
{
    float volume = 1.0f;
    //
    if([musicPlayer isPaused])
        [musicPlayer resume];
    else
    {
        [musicPlayer setVolume:volume];
        [musicPlayer playQueue];
    }
}

void OpenALAudio::SetMusicFade(float fromValue, float toValue, float time)
{
//    [musicPlayer fadeTo:toValue duration:time];
    [musicPlayer fadeFrom:fromValue to:toValue duration:time];
}
 
*/

void OpenALAudio::Pause()
{
    [sessionManager saveState];
}

void OpenALAudio::Resume()
{
    [sessionManager resumeState];
}

// end