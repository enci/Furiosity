////////////////////////////////////////////////////////////////////////////////
//  BackgroundDownloader.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 10/7/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "BackgroundDownloader.h"
#include "Defines.h"
#include "Utils.h"
#include "Resource.h"

#ifdef IOS

//#define VERBOSE

using namespace std;
using namespace Furiosity;



void Furiosity::DownloadInBackground(string     url,
                                     string     filename,
                                     void*      sender,
                                     Callback   callback,
                                     void*      data)
{
#ifdef VERBOSE
    LOG("MainThread: Dowload request for file: %s\n", url.c_str() );
#endif
    
    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
#ifdef VERBOSE
        LOG("BgThread: Dowload request for file: %s\n", url.c_str() );
#endif
        sleep(1);
        
        NSString *stringURL = [NSString stringWithUTF8String:url.c_str()];
        NSURL  *nsurl = [NSURL URLWithString:stringURL];
        NSData *urlData = [NSData dataWithContentsOfURL:nsurl];
        if ( urlData )
        {
            NSString* filePath = [NSString stringWithUTF8String:filename.c_str()];
            BOOL res = [urlData writeToFile:filePath atomically:YES];
            _UNSUED(res);
#ifdef VERBOSE
            LOG("BgThread: Saving to file: %s sucess:%i \n",
                [filePath cStringUsingEncoding:NSUTF8StringEncoding], res);
#endif
            
            // Tell the caller that the data is downloaded
            (*callback)(sender, data);
        }
#ifdef VERBOSE
        else
        {
            LOG("BgThread: Failed download of file: %s\n", url.c_str());
        }
#endif
        
        dispatch_async( dispatch_get_main_queue(), ^{
            // Add code here to update the UI/send notifications based on the
            // results of the background processing
            //
#ifdef VERBOSE
            LOG("BackgroundThread: Dowload finished for file: %s\n", url.c_str() );
#endif
        });
    });
#ifdef VERBOSE
    LOG("MainThread: Requested file: %s\n", url.c_str() );
#endif
}

#endif