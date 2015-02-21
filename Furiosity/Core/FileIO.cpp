////////////////////////////////////////////////////////////////////////////////
//  FileIO.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 5/20/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "FileIO.h"

// Framework includes
#include <string>
#include <sstream>
#include <fstream>

#ifdef ANDROID
#	include <android/asset_manager.h>
#endif

#include "ResourceManager.h"
#include "logging.h"
#include "RC4.h"

using namespace std;
using namespace Furiosity;

//#define DEBUG_ENCRYPTION


////////////////////////////////////////////////////////////////////////////////
// ReadFileAndroid
////////////////////////////////////////////////////////////////////////////////
#ifdef ANDROID
string ReadFileAndroid(const string& filename)
{
    AAsset* asset = AAssetManager_open(gResourceManager.GetAndroidAssetManager(), filename.c_str(),
                                       AASSET_MODE_UNKNOWN);

    if (asset)
    {
        // NB: 'AAsset_openFileDescriptor' only works for non-compressed files. Rather than guess
        // whether something is compressed or not, we handle it as a compressed asset. This will
        // always work for both cases.

        off_t length   = AAsset_getLength(asset);
        char* memblock = new char[length];
        AAsset_read(asset, memblock, length);
        AAsset_close(asset);

        // Passing a length parameter will solve any null-terminated issues.
        std::string str(memblock, length);
        delete[] memblock;

        return str;
    }
#ifdef DEBUG
    else
    {
        LOG("Furiosity::ReadFile() Could not open: '%s' - file not found.", filename.c_str());
    }
#endif

    return "";
}
#endif // ANDROID


////////////////////////////////////////////////////////////////////////////////
// Read file
////////////////////////////////////////////////////////////////////////////////
string Furiosity::ReadFile(const string& filename)
{
#ifdef ANDROID

    // If the file is located inside the APK use the Android AssetManager
    // to read said file. As a debug feature, files located on the sdcard
    // are read directly - this to by pass huge lag when deploying large
    // assets.

    if( gResourceManager.IsAndroidApkPath(filename)) {
        return ReadFileAndroid(filename);
    }


#endif // ANDROID
    
    uint length;
    char* buffer;
    
    ifstream is;
    is.open(filename.c_str(), ifstream::in);
    
    // Check for bad paths
    if(!is.is_open())
    {
#ifdef DEBUG
        LOG("Unable to open file: %s\n", filename.c_str());
#endif
        return "";
    }
    else
    {
        //printf("Read file: %s\n", filename);
    }
    
    // get length of file:
    is.seekg (0, ios::end);
    length = (uint)is.tellg();
    is.seekg (0, ios::beg);
    
    // allocate memory
    buffer = new char [length];
    
    // read data as a block:
    is.read(buffer,length);
    is.close();
    
    // NB: specifying a length parameter will solve any null byte issues, even when
    // reading arbitrary binary data.
    string ret = string(buffer, length);
    delete[] buffer;
    
    return ret;
}


///////////////////////////////////////////////////////////////////////////////
// Save text to a file
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::SaveFile(const std::string& filename,
                         const std::string& text)
{
    ofstream os;
    os.open(filename.c_str(), ofstream::out);
    
    // Check for bad paths
    if(!os.is_open())
    {
#ifdef DEBUG
        LOG("Unable to open file for writing: %s\n", filename.c_str());
#endif
        return false;
    }

    
    // get length of file:
    os.write(text.c_str(), text.size());
    
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Read a text file into a string
////////////////////////////////////////////////////////////////////////////////
string Furiosity::ReadEncryptedFile(const std::string& filename,
                                    const std::string& key)
{
    RC4 rc4;
    string encrypted = ReadFile(filename);
    string text = rc4.Decrypt(encrypted, key);

#ifdef DEBUG_ENCRYPTION
    LOG("Encrypted:\n%s\n", encrypted.c_str());
    LOG("Key: %s\n", key.c_str());
    LOG("Text:\n%s\n", text.c_str());
#endif
    
    return text;
}

////////////////////////////////////////////////////////////////////////////////
// Save text to a file
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::SaveEncryptedFile(const std::string& filename,
                                  const std::string& text,
                                  const std::string& key)
{
    RC4 rc4;
    string encrypted = rc4.Encrypt(text, key);
    
#ifdef DEBUG_ENCRYPTION
    LOG("Text:\n%s\n", text.c_str());
    LOG("Key: %s\n", key.c_str());
    LOG("Encrypted:\n%s\n", encrypted.c_str());
#endif
    
    return SaveFile(filename, encrypted);
}
