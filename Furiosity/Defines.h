////////////////////////////////////////////////////////////////////////////////
//  Defines.h
//  Furiosity
//
//  Created by Bojan Endrovski on 07/06/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gl.h"


// Defines for apple supported platforms
#ifdef __APPLE__
#include "TargetConditionals.h"

    #if TARGET_OS_IPHONE
        #define IOS

    #elif TARGET_IPHONE_SIMULATOR
        #define IOS

    #endif

#endif

#define USE_ASSIMP 0

namespace Furiosity
{
    typedef unsigned int    uint;
    typedef unsigned short  ushort;
    // typedef unsigned long   ulong;
    
    // A macro to call pointers to member functions
    #define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

    template< class T > void SafeDelete( T*& pVal )
    {
        if(pVal != NULL)
            delete pVal;
        pVal = NULL;
    }

    template< class T > void SafeDeleteArray( T*& pVal )
    {
        if(pVal != NULL)
            delete[] pVal;
        pVal = NULL;
    }
}
    

