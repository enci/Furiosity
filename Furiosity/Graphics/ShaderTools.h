////////////////////////////////////////////////////////////////////////////////
//  ShaderTools.h
//
//  Created by Bojan Endrovski on 10/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef SHADERTOOLS_H
#define SHADERTOOLS_H

#include "gl.h"

namespace Furiosity
{
    bool CompileShader(GLuint* shader, GLenum type, const GLchar* source);
    
    bool LinkProgram(GLuint prog);
    
    bool ValidateProgram(GLuint prog);
}

#endif
