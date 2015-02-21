////////////////////////////////////////////////////////////////////////////////
//  BackgroundDownloader.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/7/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Defines.h"

#ifdef IOS

#include <string>

namespace Furiosity
{
    /// typedef a pointer to static member method
    typedef void (*Callback)(void* sender, void* data);

    /// Convinient method to download stuff in the background, store it localy
    /// in a file and get a callback  when done
    /// TODO: Replace this with an event like the rest of the engine
    ///
    /// @param url Remote URL
    /// @param filename Local relative file name
    /// @param sender Sender of this call
    /// @param callback Callback function
    /// @oaram data Blind data
    void DownloadInBackground(std::string   url,
                              std::string   filename,
                              void*         sender,
                              Callback      callback,
                              void*         data);
}

#endif