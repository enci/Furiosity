////////////////////////////////////////////////////////////////////////////////
//  SoundResource.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 05/10/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "SoundResource.h"
#include "OpenALSupport.h"
#include "ResourceManager.h"
#include "FileIO.h"
#include "AudioManager.h"

#ifdef ANDROID
#	include "OpenSLDecode.h"
#endif

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                              Sound Buffer Data
//
////////////////////////////////////////////////////////////////////////////////

SoundResource::SoundResource(const std::string& filename) :
    Resource(RESOURCE_TYPE_SOUND),
    openalDataBuffer(0),
    openalFormat(0),
    sampleRate(0)
{
#ifdef ANDROID
    //std::string data = Furiosity::ReadFile(filename);

    bool success = Furiosity::DecodeToPCM(this, filename);

    if( ! success) {
        ERROR("Error decoding sound file:" /* + filename.c_str() */);
    }
    

#else
    
    ALenum error = 0;
    
    alGetError();   // clear errors
    alGenBuffers(1, &openalDataBuffer);
    //
    error = alGetError();
    if(error != AL_NO_ERROR)
    {
        printf("Failed to create OpenAL data buffer: %s\n", alGetString(error));
    }
    
    NSString* filenamestr   = [NSString stringWithUTF8String:filename.c_str()];
    CFURLRef fileURL        = (CFURLRef)[[NSURL fileURLWithPath:filenamestr] retain];
    
    if (fileURL)
	{	
		pcmBytes = MyGetOpenALAudioData(fileURL, &openalFormat, &sampleRate);
        //
		CFRelease(fileURL);
        
        // Set size
        size = (int) pcmBytes.size();
		
		if((error = alGetError()) != AL_NO_ERROR)
        {
			printf("Error loading sound: %x\n", error);
			exit(1);
		}
		
		// Use the static buffer data API
		alBufferDataStaticProc(openalDataBuffer, openalFormat, &pcmBytes[0], (int) pcmBytes.size(), sampleRate);
		
		if((error = alGetError()) != AL_NO_ERROR)
        {
			printf("Error attaching audio to buffer: %x\n", error);
		}
        
        
        
        // Caclculate duration
        ALint sizeInBytes;
        ALint channels;
        ALint bits;
        //
        alGetBufferi(openalDataBuffer, AL_SIZE, &sizeInBytes);
        alGetBufferi(openalDataBuffer, AL_CHANNELS, &channels);
        alGetBufferi(openalDataBuffer, AL_BITS, &bits);
        int lengthInSamples = sizeInBytes * 8 / (channels * bits);
        //
        ALint frequency;
        alGetBufferi(openalDataBuffer, AL_FREQUENCY, &frequency);
        //
        duration = (float)lengthInSamples / (float)frequency;
        size = sizeInBytes;
	}
	else
		LOG("Could not find file: %s!\n", filename.c_str());
    
   // [filenamestr release];

#endif
}

SoundResource::~SoundResource()
{
#ifndef ANDROID
    alDeleteBuffers(1, &openalDataBuffer);
#endif
}


////////////////////////////////////////////////////////////////////////////////
//
//                              Sound
//
////////////////////////////////////////////////////////////////////////////////

Sound::Sound(SoundResource* snd) : soundBuffer(snd)
{
    // TODO: See if this is really necessary
    gResourceManager.RetainResource(snd);

#ifdef ANDROID
	seekOffset = 0;
#endif
}


Sound::Sound(const string& filename)
{
    soundBuffer = gResourceManager.LoadSoundData(filename);

#ifdef ANDROID
	seekOffset = 0;
#endif
}

Sound::~Sound()
{
    gAudioManager.StopSound(this);
    gResourceManager.ReleaseResource(soundBuffer);
}

void Sound::Play()
{
    gAudioManager.PlaySound(this);
}

void Sound::Stop()
{
    gAudioManager.StopSound(this);
}

void Sound::Pause()
{
    gAudioManager.PauseSound(this);
}

void Sound::Resume()
{
    gAudioManager.ResumeSound(this);
}

void Sound::Seek(float time)
{
    gAudioManager.SeekSound(this, time);
}

float Sound::CurrentTime()
{
    return gAudioManager.GetSoundTime(this);
}

SoundState Sound::GetState()
{
    return gAudioManager.GetSoundState(this);
}

// end
