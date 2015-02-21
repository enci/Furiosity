////////////////////////////////////////////////////////////////////////////////
//  ShaderTools.cpp
//
//  Created by Bojan Endrovski on 10/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ShaderTools.h"

// Get all the basics
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include "logging.h"
#include "Defines.h"

////////////////////////////////////////////////////////////////////////////////
// Compile shader and report success or failure
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::CompileShader(GLuint* shader, GLenum type, const GLchar* source)
{
	GLint status;
    
    if (!source)
    {
    	LOG("Failed to load shader %s \n", source );
        return false;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength = 0;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        LOG("Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return false;
    }
    
    // assert(glGetError() == GL_NO_ERROR);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Link Program
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::LinkProgram(GLuint prog)
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        LOG("Program link log:\n%s", log);
        free(log);
    }
#endif
    


    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return false;
    
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Validate program
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::ValidateProgram(GLuint prog)
{
    GLint logLength = 0;
    GLint status = 0;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        //LOG("Furiosity::ValidateProgram(%i) log (%i characters) :%s", prog, logLength, log);
        free(log);
    }
    
    logLength = 0;
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
    {
    	return false;
    }
    return true;
}
