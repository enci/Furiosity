////////////////////////////////////////////////////////////////////////////////
//  gl.cpp
//
//  Created by Bojan Endrovski on 4/12/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "gl.h"
#include <string>

using namespace std;

#ifdef DEBUG

string Furiosity::GLErrorString(GLenum error)
{
    switch( error )
    {
		case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
            
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
            
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
            
		case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
            
		case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
            
		default:
            return "UNKNOWN";
    }
    
    return "UNKNOWN";
}

#endif