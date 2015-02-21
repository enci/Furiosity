#pragma once

#ifdef ANDROID

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidMetadata.h>
#include <cstdlib>

#include <condition_variable>
#include <chrono>
#include <mutex>
#include "Utils.h"
#include "FileIO.h"
#include "SoundResource.h"

//
#include "ResourceManager.h"

#include "OpenSLAudio.h"


// https://android.googlesource.com/platform/frameworks/wilhelm/+/master/tests/examples/slesTestDecodeToBuffQueue.cpp

namespace Furiosity {


	namespace Internal {
		/// Actual structure of a PCM WAVE file. Courtesy of http://stackover
		/// flow.com/questions/11936702/reading-wav-files-not-reading-header
		struct Wave {
			struct Riff {
				char chunkId[4];      // "RIFF" (assuming char is 8 bits)
				int32_t chunkSize;    //
				char format[4];       // "WAVE"
			} riffHeader;
			struct Format {
				char chunkId[4];      // "fmt "
				int32_t chunkSize;
				short format;         // assuming short is 16 bits
				short numChannels;
				int32_t sampleRate;
				int32_t byteRate;
				short align;
				short bitsPerSample;
			} formatHeader;
			struct Chunk {
				char chunkId[4];      // Identifier string
				int32_t chunkSize;    // Length of data
				char* data;           // The actual data
			} dataHeader;
		};

		struct ByteAccumulator
		{
			/// Bytes to read each time
			const int readChunkSize{262144};// * 2 * 2 * 2};

			/// Read attempts thus far
			int readCount{1};

			/// Accumulate buffers over time
			std::vector<char> pcm;

			/// Mutex is required for the condition variable.
			std::mutex mutex;

			/// Signal used from the call back to communicate with the
			/// game main thread.
			std::condition_variable condition;

			/// Interface to determine stream properties
			SLMetadataExtractionItf& metaInterface;

			SLPlayItf& playerInterface;

			unsigned int sampleRate{44100};
			unsigned int numChannels{1};
			unsigned int bitsPerSample{16};
			unsigned int containerSize{16};
			unsigned int channelMask{1};
			unsigned int endianness{2};

			ByteAccumulator(SLPlayItf& playerInterface, SLMetadataExtractionItf& metaInterface)
				: playerInterface(playerInterface)
			    , metaInterface(metaInterface)
			{
				pcm.reserve(15000000);
				pcm.resize(readChunkSize);
			}
		};


		static void EnqueueCallback(SLAndroidSimpleBufferQueueItf queueInterface, void* context)
		{
			SLresult result;

			Internal::ByteAccumulator* accumulator = static_cast<Internal::ByteAccumulator*>(context);



			SLmillisecond duration;
			result = (*accumulator->playerInterface)->GetDuration(accumulator->playerInterface, &duration);

			SLmillisecond progress;
			result = (*accumulator->playerInterface)->GetPosition(accumulator->playerInterface, &progress);

			LOG("Progress: %d / %d (%d)%%", (int) progress, (int) duration, int(100.0f / duration * progress));

			std::unique_lock<std::mutex> lock(accumulator->mutex);

			// Current offset
			const size_t startOffset = accumulator->pcm.size();

			// First run, grow by expected byte size. Adding some
			// bytes to account for floating point issues.
			if(accumulator->readCount == 1) {
				const int expectedByteSize = Furiosity::Internal::PcmByteOffset(duration * 0.001f) + 1024;

				accumulator->pcm.resize(expectedByteSize);

			// Subsequent runs, grow by a fixed size. This is generally the case
			// with non mono signals.
			} else {
				accumulator->pcm.resize(accumulator->pcm.size() + accumulator->readChunkSize);
			}

			accumulator->readCount++;

			const int readSize = accumulator->pcm.size() - startOffset;

			// Feed some more bytes
			result = (*queueInterface)->Enqueue(queueInterface, &accumulator->pcm[startOffset], readSize);
			ASSERT(result == SL_RESULT_SUCCESS);

			// Signal the main loop that something happened.
			accumulator->condition.notify_all();
		}
	}

	static bool DecodeToPCM(SoundResource* soundBuffer, const std::string& filename)
	{
		LOG("DecodeToPCM(%s)", filename.c_str());

		// Special case for WAV files, OpenSL cannot decode those.
		if(StringEndsWith(filename, ".wav"))
		{
			std::vector<char> pcmBytes;

			std::string data = Furiosity::ReadFile(filename);

			if(!data.empty())
			{
				Internal::Wave* wav = (Internal::Wave*) &data[0];

				// Uncertain if this is reliable.
				soundBuffer->numChannels   = wav->formatHeader.numChannels;
				soundBuffer->sampleRate    = wav->formatHeader.sampleRate;
				soundBuffer->bitsPerSample = wav->formatHeader.bitsPerSample;
				soundBuffer->containerSize = wav->formatHeader.byteRate;
				soundBuffer->channelMask   = 0; // Which speakers?
				soundBuffer->endianness    = wav->formatHeader.align;

				// Default offset, this works for sane audio exporters. Note that
				// sizeof(Wave); doesn't work b.c. it includes a character pointer.
				int offset = 44;

				// A chunk has 4 characters, followed by a 32 bit int indicating data length
				const int chunkHeaderSize = sizeof(char) * 4 + sizeof(int32_t);

				// Apple may add this chunk with nullbytes
				if(strncmp("FLLR", wav->dataHeader.chunkId, 4) == 0)
				{
					// Skip byte-offset ahead.
					offset += wav->dataHeader.chunkSize + chunkHeaderSize;
				}

				pcmBytes.resize(data.size() - offset);

				// Copy the bytes into a vector
				for(int i = offset, j = 0; i < data.size(); ++i, ++j)
				{
					pcmBytes[j] = data[i];
				}

				soundBuffer->setPcm(std::move(pcmBytes));

				return true;
			}

			ERROR("DecodeToPCM(%s) - file is empty or does not exist.", filename.c_str());
			return false;
		}


		// Generic variable to capture OpenSL's return state.
		SLresult result;


        SLObjectItf             engineObject;
        SLEngineItf             engineEngine;
        SLObjectItf             outputMixObject;

	    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	    ASSERT(SL_RESULT_SUCCESS == result);

	    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	    ASSERT(SL_RESULT_SUCCESS == result);

	    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	    ASSERT(SL_RESULT_SUCCESS == result);

	    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
	    ASSERT(SL_RESULT_SUCCESS == result);


	    // In my experience; it doesn't quite matter what values you enter here.
	    SLDataFormat_PCM format_pcm = {
	                SL_DATAFORMAT_PCM,              // formatType
	                1,                              // numChannels
	                SL_SAMPLINGRATE_32,
	                SL_PCMSAMPLEFORMAT_FIXED_16,    // bitsPerSample
	                16,                             // containerSize
	                SL_SPEAKER_FRONT_CENTER,        // channelMask
	                SL_BYTEORDER_LITTLEENDIAN       // endianness
	    };


		SLDataFormat_MIME dataFormat = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
		SLDataSource audioSrc;
		audioSrc.pFormat = &dataFormat;
		SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1};
		SLDataSink audioSnk = { &loc_bq, &format_pcm };

		const SLInterfaceID ids[] = {SL_IID_PLAY, SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
		const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

		SLObjectItf player;

		const SLInterfaceID ids1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_PLAY, SL_IID_METADATAEXTRACTION, SL_IID_MUTESOLO, SL_IID_PREFETCHSTATUS};
		const SLboolean req1[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

		if( ! Furiosity::gResourceManager.IsAndroidApkPath(filename)) {

			// Read from SDcard directly
			SLDataLocator_URI loc_bufq = {
				SL_DATALOCATOR_URI,
				(SLchar*) filename.c_str(),
			};

			audioSrc.pLocator = &loc_bufq;
			result = (*engineEngine)->CreateAudioPlayer(engineEngine, &player, &audioSrc, &audioSnk, 4, ids1, req1);
			ASSERT(SL_RESULT_SUCCESS == result);

		} else {
			AAsset* asset = AAssetManager_open(Furiosity::gResourceManager.GetAndroidAssetManager(), filename.c_str(), AASSET_MODE_UNKNOWN);

			if(asset == NULL) {
				ERROR("DecodeToPCM(%s) - file not found in the APK, or unable to open.", filename.c_str());
				return false;
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

			result = (*engineEngine)->CreateAudioPlayer(engineEngine, &player, &audioSrc, &audioSnk, 4, ids1, req1);
			ASSERT(SL_RESULT_SUCCESS == result);
		}

		// realize the player
		result = (*player)->Realize(player, SL_BOOLEAN_FALSE);
		ASSERT(SL_RESULT_SUCCESS == result);

		SLPlayItf playerInterface;

		// get the play interface
		result = (*player)->GetInterface(player, SL_IID_PLAY, &playerInterface);
		ASSERT(SL_RESULT_SUCCESS == result);

		//SLBufferQueueItf fdPlayerBufferQueue;
		SLAndroidSimpleBufferQueueItf fdPlayerBufferQueue;

		// get the buffer queue interface
		result = (*player)->GetInterface(player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &fdPlayerBufferQueue);
		ASSERT(SL_RESULT_SUCCESS == result);


        SLMetadataExtractionItf metaInterface;
        result = (*player)->GetInterface(player, SL_IID_METADATAEXTRACTION, &metaInterface);
        ASSERT(SL_RESULT_SUCCESS == result);


        //SLMetadataExtractionItf metaInterface;
        //result = (*player)->GetInterface(outputMixObject, SL_IID_METADATAEXTRACTION, &metaInterface);
        //ASSERT(SL_RESULT_SUCCESS == result);



        SLMuteSoloItf muteSoloItf;

        result = (*player)->GetInterface(player, SL_IID_MUTESOLO, &muteSoloItf);
        ASSERT(SL_RESULT_SUCCESS == result);


        SLPrefetchStatusItf prefetchItf;
        //result = (*player)->GetInterface(player, SL_IID_PREFETCHSTATUS, (void*)&prefetchItf);
        ASSERT(SL_RESULT_SUCCESS == result);

        //result = (*prefetchItf)->RegisterCallback(prefetchItf, PrefetchEventCallback, &prefetchItf);
		ASSERT(SL_RESULT_SUCCESS == result);

		//result = (*prefetchItf)->SetCallbackEventsMask(prefetchItf, SL_PREFETCHEVENT_FILLLEVELCHANGE | SL_PREFETCHEVENT_STATUSCHANGE);
		ASSERT(SL_RESULT_SUCCESS == result);

		Internal::ByteAccumulator accumulator(playerInterface, metaInterface);

		// register callback on the buffer queue
        result = (*fdPlayerBufferQueue)->RegisterCallback(fdPlayerBufferQueue, &Internal::EnqueueCallback, &accumulator);
        ASSERT(SL_RESULT_SUCCESS == result);


        // Bytes nomnomnom.
        result = (*fdPlayerBufferQueue)->Enqueue(fdPlayerBufferQueue, &accumulator.pcm[0], accumulator.readChunkSize);
        ASSERT(SL_RESULT_SUCCESS == result);

        result = (*playerInterface)->SetPlayState(playerInterface, SL_PLAYSTATE_PLAYING);
        ASSERT(SL_RESULT_SUCCESS == result);



        // TODO: There's a theoretical deadlock here.

        SLuint32 state;
        do {
        	std::unique_lock<std::mutex> lock(accumulator.mutex);

            //SLuint8 numChannels;
            //result = (*muteSoloItf)->GetNumChannels(muteSoloItf, &numChannels);
            //LOG("===========Number of channels: %d", (int) numChannels);
            //ASSERT(SL_RESULT_SUCCESS == result);

            //result = (*muteSoloItf)->SetChannelMute(muteSoloItf, 0, SL_BOOLEAN_TRUE);
            //ASSERT(SL_RESULT_SUCCESS == result);

        	// Wait until OpenSL has a buffer event.
        	accumulator.condition.wait_for(lock, std::chrono::milliseconds(250));

			result = (*playerInterface)->GetPlayState(playerInterface, &state);
			ASSERT(SL_RESULT_SUCCESS == result);

			//LOG("State = %u", state);

        } while(state == SL_PLAYSTATE_PLAYING);


		SLmillisecond duration;
		result = (*playerInterface)->GetDuration(playerInterface, &duration);

		const int expectedByteSize = Furiosity::Internal::PcmByteOffset(duration * 0.001f);


	    // Special tuned heuristic for detecting stereo.
	    if(accumulator.pcm.size() > expectedByteSize * 1.5f) {
	    	std::vector<char> mono; mono.reserve(accumulator.pcm.size() * 0.5);

	    	for(size_t i = 0; i < accumulator.pcm.size() - 4; i += 4) {

	    		mono.push_back(accumulator.pcm[i + 0]);
	    		mono.push_back(accumulator.pcm[i + 1]);
	    	}

	    	accumulator.pcm = std::move(mono);

	    	LOG("Applied mono hack; removed the stereo channel.");
	    }

		// Stop playing, for good measure.
	    (*playerInterface)->SetPlayState(playerInterface, SL_PLAYSTATE_STOPPED);
	    (*player)->Destroy(player);
	    (*outputMixObject)->Destroy(outputMixObject);
	    (*engineObject)->Destroy(engineObject);


	    // Trim any trailing padding bytes.
		accumulator.pcm.resize(expectedByteSize);


	    // A vector seems to grow grows exponentially, this makes sense.
	    accumulator.pcm.shrink_to_fit();
	    soundBuffer->setPcm(std::move(accumulator.pcm));

		LOG("Terminating with %d of %d bytes and %d bytes wasted space. Goodbye commander.",
				(int)accumulator.pcm.size(), expectedByteSize,
				int(accumulator.pcm.capacity() - accumulator.pcm.size())
		);

		//exit(0);
		return true;
	}
}




#endif
