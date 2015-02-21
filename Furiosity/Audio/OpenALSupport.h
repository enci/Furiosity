////////////////////////////////////////////////////////////////////////////////
//  OpenALSupport.h
//  Furiosity
//
//  Taken from apple's samples. Used without warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENAL_SUPPORT_H
#define OPENAL_SUPPORT_H

#include "Defines.h"

// IOS only.
#ifdef IOS

#include <vector>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/ExtendedAudioFile.h>

typedef ALvoid	AL_APIENTRY	(*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
ALvoid  alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

std::vector<char> MyGetOpenALAudioData(CFURLRef inFileURL, ALenum *outDataFormat, ALsizei*	outSampleRate);

#endif

#endif
