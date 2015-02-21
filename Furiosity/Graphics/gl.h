////////////////////////////////////////////////////////////////////////////////
//  gl.cpp
//
//  Created by Bojan Endrovski on 4/12/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


// Platform specific OpenGL ES 2 includes for Android and iOS
#ifdef ANDROID
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#else
#	include <OpenGLES/ES2/gl.h>
#	include <OpenGLES/ES2/glext.h>
#endif

#include <string>
#include "logging.h"

namespace Furiosity
{
    
#ifdef DEBUG
    
    std::string GLErrorString(GLenum error);
    
#define GL_GET_ERROR()                                              \
{                                                                   \
    GLenum error;                                                   \
    bool err = false;                                               \
    while ( (error = glGetError()) != GL_NO_ERROR)                  \
    {                                                               \
        LOG( "OpenGL ERROR: %s\nCHECK POINT: %s (line %d)\n",       \
        GLErrorString(error).c_str(), __FILE__, __LINE__ );         \
        err = true;                                                 \
    }                                                               \
    ASSERT(!err);                                                   \
}
    
#define GL_CLEAR_ERROR()                                            \
{                                                                   \
    GLenum error;                                                   \
    while ( (error = glGetError()) != GL_NO_ERROR);                 \
}
    
#else
    #define GL_CLEAR_ERROR()
    #define GL_GET_ERROR()
#endif
    
} // Furiosity