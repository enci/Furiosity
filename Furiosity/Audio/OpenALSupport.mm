////////////////////////////////////////////////////////////////////////////////
//  OpenALSupport.c
//  Furiosity
//
//  Taken from apple's samples. Used without warranty.
////////////////////////////////////////////////////////////////////////////////

#include "OpenALSupport.h"

ALvoid  alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	static	alBufferDataStaticProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    }
    
    if (proc)
        proc(bid, format, data, size, freq);
	
    return;
}

std::vector<char> MyGetOpenALAudioData(CFURLRef inFileURL, ALenum *outDataFormat, ALsizei*	outSampleRate)
{
	OSStatus						err = noErr;	
	SInt64							theFileLengthInFrames = 0;
	AudioStreamBasicDescription		theFileFormat;
	UInt32							thePropertySize = sizeof(theFileFormat);
	ExtAudioFileRef					extRef = NULL;
	AudioStreamBasicDescription		theOutputFormat;
    
    std::vector<char> pcmBytes;
    
	// Open a file with ExtAudioFileOpen()
	err = ExtAudioFileOpenURL(inFileURL, &extRef);
	if(err)
    {
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSLog(@"MyGetOpenALAudioData: ExtAudioFileOpenURL FAILED:\n------------\n%@\n------------\n", [error localizedDescription]);
        
        assert(0);
        return pcmBytes;
    }
	
	// Get the audio data format
	err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileDataFormat, &thePropertySize, &theFileFormat);
	if(err)
    {
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSLog(@"MyGetOpenALAudioData: ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat)  FAILED:\n------------\n%@\n------------\n", [error localizedDescription]);
        
        assert(0);
        return pcmBytes;
    }
	if (theFileFormat.mChannelsPerFrame > 2) 
    {
        printf("MyGetOpenALAudioData - Unsupported Format, channel count is greater than stereo\n");
        assert(0);
        return pcmBytes;
    }
    
	// Set the client format to 16 bit signed integer (native-endian) data
	// Maintain the channel count and sample rate of the original source format
    theOutputFormat.mSampleRate = theFileFormat.mSampleRate;
    //    theOutputFormat.mSampleRate = 22050;
	theOutputFormat.mChannelsPerFrame = 1;//theFileFormat.mChannelsPerFrame;
    
	theOutputFormat.mFormatID = kAudioFormatLinearPCM;
	theOutputFormat.mBytesPerPacket = 2 * theOutputFormat.mChannelsPerFrame;
	theOutputFormat.mFramesPerPacket = 1;
	theOutputFormat.mBytesPerFrame = 2 * theOutputFormat.mChannelsPerFrame;
	theOutputFormat.mBitsPerChannel = 16;
	theOutputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
	
	// Set the desired client (output) data format
	err = ExtAudioFileSetProperty(extRef, kExtAudioFileProperty_ClientDataFormat, sizeof(theOutputFormat), &theOutputFormat);
	if(err)
    {
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSLog(@"MyGetOpenALAudioData: ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat)  FAILED:\n------------\n%@\n------------\n", [error localizedDescription]);
        
        assert(0);
        return pcmBytes;
    }
	
	// Get the total frame count
	thePropertySize = sizeof(theFileLengthInFrames);
	err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileLengthFrames, &thePropertySize, &theFileLengthInFrames);
	if(err)
    {
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSLog(@"MyGetOpenALAudioData: ExtAudioFileSetProperty(kExtAudioFileProperty_FileLengthFrames)  FAILED:\n------------\n%@\n------------\n", [error localizedDescription]);
        
        assert(0);
        return pcmBytes;
    }
	
	// Calculate the byte-size
	UInt32 dataSize = (uint)theFileLengthInFrames * theOutputFormat.mBytesPerFrame;
    
    // Allocate enough space in the vector
    pcmBytes.resize(dataSize);
    

    AudioBufferList		theDataBuffer;
    theDataBuffer.mNumberBuffers = 1;
    theDataBuffer.mBuffers[0].mDataByteSize = dataSize;
    theDataBuffer.mBuffers[0].mNumberChannels = theOutputFormat.mChannelsPerFrame;
    theDataBuffer.mBuffers[0].mData = &pcmBytes[0];
    
    // Read the data into an AudioBufferList
    err = ExtAudioFileRead(extRef, (UInt32*)&theFileLengthInFrames, &theDataBuffer);
    if(err == noErr)
    {
        // success
        *outDataFormat = (theOutputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        *outSampleRate = (ALsizei)theOutputFormat.mSampleRate;
    }
    else 
    {
        // There was an error, make sure the return buffer is empty.
        pcmBytes.clear();
        
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSLog(@"MyGetOpenALAudioData: ExtAudioFileRead FAILED:\n------------\n%@\n------------\n", [error localizedDescription]);
        
        assert(0);
        return pcmBytes;
    }	
	
    
    assert(extRef);
    err = ExtAudioFileDispose(extRef);
    assert(err == noErr);

    
    // Force the move ctor. Byte buffers are quite large, skip a copy.
    return std::move(pcmBytes);

	/*
Exit:
	// Dispose the ExtAudioFileRef, it is no longer needed
	if (extRef) ExtAudioFileDispose(extRef);
	return theData;
     */
}
